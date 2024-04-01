import sys
import os
from qtpy import QtWidgets, QtGui, QtCore
import numpy as np
import pyvista as pv
from pyvistaqt import QtInteractor, MainWindow

from model import model

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

        self.setWindowTitle("FEM3D")

        # Create central frame
        self.left_vertical_splitter = QtWidgets.QSplitter()
        self.left_vertical_splitter.setOrientation(QtCore.Qt.Orientation.Vertical)
        self.horizontal_splitter = QtWidgets.QSplitter()
        self.setCentralWidget(self.horizontal_splitter)

        # Initialize object view
        self.plotter = QtInteractor(self.horizontal_splitter)
        self.signal_close.connect(self.plotter.close)

        # Create menu bar
        main_menu = self.menuBar()
        file_menu = main_menu.addMenu('File')

        open_button = QtWidgets.QAction('Open', self)
        open_button.setShortcut('Ctrl+O')
        open_button.triggered.connect(self.open_file)
        file_menu.addAction(open_button)

        exit_button = QtWidgets.QAction('Exit', self)
        exit_button.setShortcut('Ctrl+Q')
        exit_button.triggered.connect(self.close)
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

    def open_file(self):
        filename = QtWidgets.QFileDialog.getOpenFileName(self, 'Open file', './', "Step Files (*.stp *.step)")
        if filename[0] == '':
            return
        self.model = model(filename[0])
        self.model.plot(self.plotter)
        part_ids = self.model.get_part_ids()
        widget_items = []
        for id in part_ids:
            widget_items.append(QtWidgets.QTreeWidgetItem(["Body"+str(id)]))

        self.tree_solids.insertChildren(0, widget_items)

    def tree_item_clicked(self, it, col):
        if (it not in [self.tree_solids, self.tree_ports, self.tree_materials]):
            id = int(it.text(0)[-1])
            self.model.highlight_part(id)

    def tree_deselected(self):
        if (self.model is not None):
            self.model.remove_highlights()

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    window = MyMainWindow()
    sys.exit(app.exec_())