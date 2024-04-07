import sys
import os
from qtpy import QtWidgets, QtGui, QtCore
import numpy as np
import pyvista as pv
from pyvistaqt import QtInteractor, MainWindow

from model import model
from setup import setup

os.environ["QT_API"] = "pyqt5"

class DeselectableTreeWidget(QtWidgets.QTreeWidget):
    def __init__(self, deselect_callback):
        QtWidgets.QTreeWidget.__init__(self)
        self.callback = deselect_callback

    def mousePressEvent(self, event):
        self.clearSelection()
        self.callback()
        QtWidgets.QTreeWidget.mousePressEvent(self, event)

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
        self.main_menu = self.menuBar()
        file_menu = self.main_menu.addMenu('File')

        import_button = QtWidgets.QAction('Import .step file', self)
        import_button.triggered.connect(self.import_step)
        file_menu.addAction(import_button)

        open_button = QtWidgets.QAction('Open', self)
        open_button.setShortcut('Ctrl+O')
        open_button.triggered.connect(self.open_file)
        file_menu.addAction(open_button)

        exit_button = QtWidgets.QAction('Exit', self)
        exit_button.setShortcut('Ctrl+Q')
        exit_button.triggered.connect(self.close)
        file_menu.addAction(exit_button)

        save_button = QtWidgets.QAction('Save', self)
        save_button.setShortcut('Ctrl+S')
        save_button.triggered.connect(self.close)
        file_menu.addAction(exit_button)

        # Add Tree
        self.tree = DeselectableTreeWidget(self.tree_deselected)
        self.tree.setColumnCount(1)
        self.tree.setHeaderHidden(True)
        self.tree_solids = QtWidgets.QTreeWidgetItem(["Solids"])
        self.tree_ports = QtWidgets.QTreeWidgetItem(["Ports"])
        self.tree_materials = QtWidgets.QTreeWidgetItem(["Materials"])
        self.tree.insertTopLevelItems(0, [self.tree_solids, self.tree_ports, self.tree_materials])
        self.tree.itemClicked.connect(self.tree_item_clicked)

        # Add Properties Window
        self.properties = QtWidgets.QWidget()

        # Add to splitter
        self.left_vertical_splitter.addWidget(self.tree)
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
        part_ids = self.model.get_part_ids()
        widget_items = []
        for id in part_ids:
            widget_items.append(QtWidgets.QTreeWidgetItem(["Body"+str(id)]))

        edit_menu = self.main_menu.addMenu('Edit')
        select_behind_button = QtWidgets.QAction('Select Behind', self)
        select_behind_button.setShortcut('b')
        select_behind_button.triggered.connect(self.select_behind)
        edit_menu.addAction(select_behind_button)

        self.tree_solids.insertChildren(0, widget_items)

    def open_file(self):
        filename = QtWidgets.QFileDialog.getOpenFileName(self, 'Open file', './', "Json Config Files (*.json)")
        if filename[0] == '':
            return
        self.setup = setup(filename[0])

    def save_file(self):
        return

    def tree_item_clicked(self, it, col):
        if (it not in [self.tree_solids, self.tree_ports, self.tree_materials]):
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

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    window = MyMainWindow()
    window.show()
    sys.exit(app.exec_())