import sys
import os
from qtpy import QtWidgets, QtGui, QtCore
import numpy as np
from pyvistaqt import QtInteractor, MainWindow

from model import model
from setup import setup
from ui_components import *

os.environ["QT_API"] = "pyqt5"

class MyMainWindow(MainWindow):

    def __init__(self, parent=None):
        QtWidgets.QMainWindow.__init__(self, parent)
        self.model = None
        self.filename = None
        self.setup = setup()

        self.setWindowTitle("FEM3D")

        # Create central frame
        self.left_vertical_splitter = QtWidgets.QSplitter()
        self.left_vertical_splitter.setOrientation(QtCore.Qt.Orientation.Vertical)
        self.horizontal_splitter = QtWidgets.QSplitter()
        self.setCentralWidget(self.horizontal_splitter)

        # Initialize object view
        self.plotter = QtInteractor(self.horizontal_splitter)
        self.plotter.enable_surface_point_picking(callback=self.surface_selection_callback, show_point=False, show_message=False)
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
                            self.assign_port)

        # Add Tree
        self.tree = model_tree(self.tree_deselected, self.tree_item_selected)

        # Form layout
        self.left_vertical_splitter.addWidget(self.tree.widget_handle())
        self.horizontal_splitter.addWidget(self.left_vertical_splitter)
        self.horizontal_splitter.addWidget(self.plotter.interactor)
            
        self.show()

    def load_step(self, filename):
        self.plotter.clear_actors()
        self.tree.clear_solids()

        self.model = model(filename)
        self.model.plot(self.plotter)        
        self.tree.add_solids(self.model.get_part_ids())
        self.menubar.enable_edit()
        self.setup.update_from_model(self.model)

    def import_step(self):
        filename = QtWidgets.QFileDialog.getOpenFileName(self, 'Open file', './', "Step Files (*.stp *.step)")
        if filename[0] == '':
            return
        self.load_step(filename[0])

    def open_file(self):
        filename = QtWidgets.QFileDialog.getOpenFileName(self, 'Open file', './', "Json Config Files (*.json)")
        if filename[0] == '':
            return
        self.setup = setup(filename[0])
        self.load_step(self.setup.get_step_filename())
        self.tree.clear_materials()
        self.tree.add_materials(self.setup.get_materials())

    def save_file(self):
        if not self.setup.has_filename():
            filename = QtWidgets.QFileDialog.getSaveFileName(self, 'Save File', './', "Json Config Files (*.json)")
            if filename[0] == '':
                return
            self.setup.set_filename(filename[0])
        self.setup.save_setup()
    
    def remove_splitter_focus(self):
        to_delete = self.left_vertical_splitter.widget(1)
        if (to_delete is not None):
            to_delete.hide()
            to_delete.deleteLater()

    def tree_item_selected(self, it, col):
        solid_index = self.tree.get_solid_index(it)
        material_index = self.tree.get_material_index(it)

        if (solid_index > -1):
            self.model.highlight_part(solid_index)
        
        elif (material_index > -1):
            table = value_table(self.setup.get_material(it.text(0)))
            self.left_vertical_splitter.addWidget(table.widget_handle())

    def tree_deselected(self):
        self.remove_splitter_focus()
        if (self.model is not None):
            self.model.remove_highlights()

    def get_mouse_vector_and_position(self, point):
        mouse_vector = np.subtract(point, self.plotter.camera_position[0])
        mouse_vector = mouse_vector / np.linalg.norm(mouse_vector)
        return self.plotter.camera_position[0], mouse_vector

    def surface_selection_callback(self, point):
        position, mouse_vector = self.get_mouse_vector_and_position(point)
        self.model.remove_highlights()
        self.model.select_faces(position, mouse_vector)
        self.model.cycle_highlighted_face(self.plotter)

    def select_behind(self):
        self.model.cycle_highlighted_face(self.plotter)

    def create_material(self):
        dialog = material_create_dialog(self)

        while dialog.exec():
            if not self.setup.contains_material(dialog.get_result()["name"]):
                self.setup.add_material(dialog.get_result())
                self.tree.clear_materials()
                self.tree.add_materials(self.setup.get_materials())
                break
            else:
                warning = warning_dialog("Warning!", "Material with same name already exists.", dialog)
                warning.exec()

    def assign_material(self):
        selected_parts = self.model.get_highlighted_parts()
        materials = self.setup.get_materials()

        if len(selected_parts) == 0:
            warning = warning_dialog("Warning!", "No volumes selected.", self)
            warning.exec()
            return
        
        if len(materials) == 0:
            warning = warning_dialog("Warning!", "No materials available.", self)
            warning.exec()
            return
                        
        dialog = list_select_dialog("Select Material", self.setup.get_materials())
        if dialog.exec():
            self.setup.assign_material(selected_parts, dialog.get_result())

    def assign_port(self):
        print("Assigned!")

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    window = MyMainWindow()
    window.resize(1200, 700)
    window.show()
    sys.exit(app.exec_())