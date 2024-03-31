import sys
import os
from qtpy import QtWidgets, QtGui, QtCore
import numpy as np
import pyvista as pv
from pyvistaqt import QtInteractor, MainWindow

from model import model

os.environ["QT_API"] = "pyqt5"

class MyMainWindow(MainWindow):

    def __init__(self, parent=None, show=True):
        QtWidgets.QMainWindow.__init__(self, parent)

        # Create central frame
        self.splitter =  QtWidgets.QSplitter()
        self.setCentralWidget(self.splitter)

        # Initialize object view
        self.plotter = QtInteractor(self.splitter)
        self.splitter.addWidget(self.plotter.interactor)
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
        self.tree = QtWidgets.QTreeWidget()
        self.tree.setColumnCount(2)
        self.tree.setHeaderLabels(["Name", "Type"])
        self.splitter.addWidget(self.tree)

        if show:
            self.show()

    def open_file(self):
        filename = QtWidgets.QFileDialog.getOpenFileName(self, 'Open file', './', "Step Files (*.stp *.step)")
        if filename[0] == '':
            return
        self.model = model(filename[0])
        self.model.plot(self.plotter)

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    window = MyMainWindow()
    sys.exit(app.exec_())