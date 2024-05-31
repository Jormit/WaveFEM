import sys
import os
from qtpy import QtWidgets, QtCore, QtGui
import numpy as np
from pyvistaqt import QtInteractor, MainWindow
import subprocess

from model import model
from setup import setup
from results import results
import defaults
from ui_components import *

import qdarkstyle

os.environ["QT_API"] = "pyqt5"

class main_window(MainWindow):

    def __init__(self, parent=None):
        QtWidgets.QMainWindow.__init__(self, parent)
        self.model = None
        self.setup = setup()
        self.results = None
        self.surface_clicked = False
        self.focused_material = None

        self.setWindowTitle(defaults.title)
        self.setWindowIcon(QtGui.QIcon(app_icon))

        # Create central frame
        self.left_vertical_splitter = QtWidgets.QSplitter()
        self.left_vertical_splitter.setOrientation(QtCore.Qt.Orientation.Vertical)
        self.right_vertical_splitter = QtWidgets.QSplitter()
        self.right_vertical_splitter.setOrientation(QtCore.Qt.Orientation.Vertical)

        self.horizontal_splitter = QtWidgets.QSplitter()
        self.setCentralWidget(self.horizontal_splitter)

        # Initialize object view
        self.plotter = QtInteractor(self.right_vertical_splitter)
        self.plotter.set_background("white")
        self.plotter.enable_surface_point_picking(callback=self.surface_selection_callback, show_point=False, show_message=False, tolerance=0.001)
        self.plotter.track_click_position(callback=self.plotter_click_callback)

        self.signal_close.connect(self.plotter.close)

        # Create menu bar
        self.menubar = menu(self,
                            self.open_file, 
                            self.save_file,
                            self.import_step,
                            self.close,
                            self.select_behind,
                            self.create_material,
                            self.assign_material,
                            self.assign_port,
                            self.validate_simulation,
                            self.run_simulation)

        # Add Tree
        self.tree = model_tree(self.tree_deselected, self.tree_item_selected)

        # Add Console Box
        self.console = console_box()
        sys.stdout.write = self.console.print_text

        # Form layout
        self.right_vertical_splitter.addWidget(self.plotter)
        self.right_vertical_splitter.addWidget(self.console.widget_handle())
        self.left_vertical_splitter.addWidget(self.tree.widget_handle())

        self.horizontal_splitter.addWidget(self.left_vertical_splitter)
        self.horizontal_splitter.addWidget(self.right_vertical_splitter)
        
        self.horizontal_splitter.setSizes([int(self.frameGeometry().width() * 0.15), int(self.frameGeometry().width() * 0.85)])
        self.right_vertical_splitter.setSizes([int(self.frameGeometry().width() * 0.8), int(self.frameGeometry().width() * 0.2)])
            
        self.showMaximized()

    def load_step(self, filename, new_project):
        self.plotter.clear_actors()
        self.plotter.clear_plane_widgets()
        self.tree.clear_ports()

        self.model = model(filename)

        if (new_project):
            self.setup.update_from_model(self.model)

        self.model.plot(self.plotter)        
        self.tree.set_solids(self.setup.get_part_ids())
        self.menubar.enable_edit()

        print("Loaded {}".format(filename))

    def import_step(self):
        filename = QtWidgets.QFileDialog.getOpenFileName(self, 'Open file', './', "Step Files (*.stp *.step)")[0]
        if filename == '':
            return
        self.load_step(filename, True)

    def open_file(self):
        filename = QtWidgets.QFileDialog.getOpenFileName(self, 'Open file', './', "Json Config Files (*.json)")[0]
        if filename == '':
            return
        self.setup = setup(filename)
        self.load_step(self.setup.get_step_filename(), False)
        self.results = results(self.setup.results_directory())

        self.tree.set_materials(self.setup.get_materials())
        self.tree.set_number_of_ports(self.setup.num_ports())
        self.tree.set_results(self.results.results_list())
        print("Loaded {}".format(filename))

    def save_file(self):
        if not self.setup.has_filename():
            filename = QtWidgets.QFileDialog.getSaveFileName(self, 'Save File', './', "Json Config Files (*.json)")[0]
            if filename == '':
                return
            self.setup.set_filename(filename)
        self.setup.save_setup()

        if self.model is not None:
            self.model.move_file_to(self.setup.get_step_filename())

        print("Saved {}".format(self.setup.filename))
    
    def remove_splitter_focus(self):
        to_delete = self.left_vertical_splitter.widget(1)
        if (to_delete is not None):
            to_delete.hide()
            to_delete.deleteLater()

    def tree_item_selected(self, it, col):
        solid_index = self.tree.get_solid_index(it)
        material_index = self.tree.get_material_index(it)
        port_index = self.tree.get_port_index(it)
        result_index = self.tree.get_result_index(it)
        is_setup = self.tree.is_setup(it)

        if (solid_index > -1):
            self.model.reset_shading(0.3)
            self.model.highlight_part(solid_index)
            self.menubar.enable_material_assignment()
        
        elif (material_index > -1):
            self.focused_material = it.text(0)
            table = value_table_with_edit_and_delete_button(self.setup.get_material(self.focused_material), self.edit_material, self.delete_material)
            self.left_vertical_splitter.addWidget(table.widget_handle())

        elif (port_index > -1):
            self.model.highlight_faces_in_bounding_box(self.setup.get_port(port_index))
        
        elif (is_setup):
            table = value_table_with_edit_and_delete_button(self.setup.get_misc_params(), self.edit_setup, None)
            self.left_vertical_splitter.addWidget(table.widget_handle())

        elif (result_index > -1):
            self.model.reset_shading(0.1)
            self.results.activate_dataset(it.text(0), self.plotter)

            table = value_table_with_edit_and_delete_button(self.results.dataset_parameters(), self.edit_results, None)
            self.left_vertical_splitter.addWidget(table.widget_handle())

    def tree_deselected(self):
        self.remove_splitter_focus()
        if (self.model is not None):
            self.model.remove_highlights()
        if (self.results is not None):
            self.results.deactivate_dataset(self.plotter)
        self.menubar.disable_assignment()

    def get_mouse_vector_and_position(self, point):
        mouse_vector = np.subtract(point, self.plotter.camera_position[0])
        mouse_vector = mouse_vector / np.linalg.norm(mouse_vector)
        return self.plotter.camera_position[0], mouse_vector

    def plotter_click_callback(self, point):
        if (self.surface_clicked is False):
            self.model.remove_highlights()
            self.menubar.disable_assignment()
        if (self.results is not None):
            self.results.deactivate_dataset(self.plotter)
        self.surface_clicked = False        

    def surface_selection_callback(self, point):
        position, mouse_vector = self.get_mouse_vector_and_position(point)
        self.model.remove_highlights()
        self.model.select_faces(position, mouse_vector)
        self.model.cycle_highlighted_face(self.plotter)
        self.surface_clicked = True
        self.menubar.enable_port_assignment()

    def select_behind(self):
        self.model.cycle_highlighted_face(self.plotter)

    def create_material(self):
        dialog = table_create_dialog(dict(defaults.material), "Create Material", self)

        while dialog.exec():
            if not self.setup.contains_material(dialog.get_result()["name"]):
                self.setup.add_material(dialog.get_result())
                self.tree.set_materials(self.setup.get_materials())
                break
            else:
                warning = warning_dialog("Warning!", "Material with same name already exists.", dialog)
                warning.exec()

    def edit_material(self):
        dialog = table_create_dialog(self.setup.get_material(self.focused_material), "Edit Material", self)
        new_name = None
        while dialog.exec():
            new_name = dialog.get_result()["name"]
            if new_name == self.focused_material:
                self.setup.add_material(dialog.get_result())
                break
            elif not self.setup.contains_material(new_name):
                self.setup.remove_material(self.focused_material)
                self.setup.add_material(dialog.get_result())
                self.setup.update_material_assignment(self.focused_material, new_name)
                self.tree.set_solids(self.setup.get_part_ids())
                break
            else:
                warning = warning_dialog("Warning!", "Material with same name already exists.", dialog)
                warning.exec()

        if new_name is not None:
            self.tree.set_materials(self.setup.get_materials())
            self.remove_splitter_focus()            

            self.focused_material = new_name
            table = value_table_with_edit_and_delete_button(self.setup.get_material(self.focused_material), self.edit_material, self.delete_material)
            self.left_vertical_splitter.addWidget(table.widget_handle())

    def delete_material(self):
        self.setup.remove_material(self.focused_material)
        self.setup.update_material_assignment(self.focused_material, "")
        self.tree.set_materials(self.setup.get_materials())
        self.remove_splitter_focus()
        self.tree.set_solids(self.setup.get_part_ids())

    def assign_material(self):
        selected_parts = self.model.get_highlighted_parts()
        materials = self.setup.get_materials()
        
        if len(materials) == 0:
            warning = warning_dialog("Warning!", "No materials available.", self)
            warning.exec()
            return
                        
        dialog = list_select_dialog("Select Material", self.setup.get_materials())
        if dialog.exec():
            self.setup.assign_material(selected_parts, dialog.get_result())
            self.tree.set_solids(self.setup.get_part_ids())

    def assign_port(self):
        bbox = self.model.get_selected_face_bbox()
        self.setup.add_port(bbox)
        self.tree.set_number_of_ports(self.setup.num_ports())

    def edit_setup(self):
        dialog = table_create_dialog(self.setup.get_misc_params(), "Edit Setup", self)
        if dialog.exec():
            self.setup.set_misc_params(dialog.get_result())
            self.remove_splitter_focus()
            table = value_table_with_edit_and_delete_button(self.setup.get_misc_params(), self.edit_setup, None)
            self.left_vertical_splitter.addWidget(table.widget_handle())

    def edit_results(self):
        dialog = table_create_dialog(self.results.dataset_parameters(), "Edit Setup", self)
        if dialog.exec():
            self.results.set_dataset_parameters(dialog.get_result())
            self.remove_splitter_focus()
            table = value_table_with_edit_and_delete_button(self.results.dataset_parameters(), self.edit_results, None)
            self.left_vertical_splitter.addWidget(table.widget_handle())
            self.results.refresh_dataset(self.plotter)

    def validate_simulation(self):
        print("\nValidation Result:")
        is_valid = self.setup.validate()
        if is_valid:
            print("Setup is Valid!")
        else:
            print("Setup is Invalid!")
        
    def run_simulation(self):
        
        if (self.setup.validate()):
            print("Running!")
            self.save_file()
            if (self.results is not None):
                self.results.deactivate_dataset(self.plotter)
            subprocess.run([defaults.sim_location, self.setup.filename])
            self.results = results(self.setup.results_directory())
            self.tree.set_results(self.results.results_list()) 
            print("Finished!")

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    app.setStyleSheet(qdarkstyle.load_stylesheet(qdarkstyle.LightPalette))
    window = main_window()
    sys.exit(app.exec_())