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

        self.setWindowTitle("FEM3D")

        # Create central frame
        self.left_vertical_splitter = QtWidgets.QSplitter()
        self.left_vertical_splitter.setOrientation(QtCore.Qt.Orientation.Vertical)
        self.horizontal_splitter = QtWidgets.QSplitter()
        self.setCentralWidget(self.horizontal_splitter)

        # Initialize object view
        self.plotter = QtInteractor(self.horizontal_splitter)
        self.plotter.enable_surface_point_picking(callback=self.surface_selection_callback, show_point=True, font_size=10)
        self.signal_close.connect(self.plotter.close)

        # Create menu bar
        self.menubar = menu(self, self.open_file, self.save_file, self.import_step, self.close, self.select_behind, self.create_material)

        # Add Tree
        self.tree = model_tree(self.tree_deselected, self.tree_item_selected)

        # Volume properties widget
        self.volume_properties = volume_widget()

        # Add Properties Window
        self.properties = QtWidgets.QStackedWidget()
        self.properties.addWidget(self.volume_properties.widget_handle())

        # Form layout
        self.left_vertical_splitter.addWidget(self.tree.widget_handle())
        self.left_vertical_splitter.addWidget(self.properties)
        self.horizontal_splitter.addWidget(self.left_vertical_splitter)
        self.horizontal_splitter.addWidget(self.plotter.interactor)
            
        self.show()

    def import_step(self):
        filename = QtWidgets.QFileDialog.getOpenFileName(self, 'Open file', './', "Step Files (*.stp *.step)")
        if filename[0] == '':
            return
        self.model = model(filename[0])
        self.model.plot(self.plotter)
        self.tree.set_solids(self.model.get_part_ids())
        self.setup = setup()
        self.setup.model_file = os.path.basename(filename[0])
        self.menubar.enable_edit()

    def open_file(self):
        filename = QtWidgets.QFileDialog.getOpenFileName(self, 'Open file', './', "Json Config Files (*.json)")
        if filename[0] == '':
            return
        self.setup = setup(filename[0])

    def save_file(self):
        print("Saved!")
        return

    def tree_item_selected(self, it, col):
        if self.tree.is_solid_selection(it):
            id = int(it.text(0)[-1])
            self.model.highlight_part(id)

    def tree_deselected(self):
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
        dialog = material_dialog(self)
        if dialog.exec():
            self.tree.add_material([dialog.get_result()])
        else:
            print("Cancel!")

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    window = MyMainWindow()
    window.show()
    sys.exit(app.exec_())