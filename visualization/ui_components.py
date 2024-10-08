from qtpy import QtWidgets, QtGui, QtCore

import matplotlib
matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg
from matplotlib.figure import Figure

app_icon = "./visualization/assets/icons/application-wave.png"
save_icon = "./visualization/assets/icons/disk.png"
open_icon = "./visualization/assets/icons/folder.png"
import_icon = "./visualization/assets/icons/application-import.png"
close_icon = "./visualization/assets/icons/cross.png"
select_behind_icon = "./visualization/assets/icons/arrow-return.png"
warning_icon = "./visualization/assets/icons/exclamation.png"
validate_icon = "./visualization/assets/icons/tick.png"
run_icon = "./visualization/assets/icons/arrow.png"

solids_icon = "./visualization/assets/icons/application-resize.png"
ports_icon = "./visualization/assets/icons/slide.png"
materials_icon = "./visualization/assets/icons/ice.png"
setup_icon = "./visualization/assets/icons/gear.png"
results_icon = "./visualization/assets/icons/document-table.png"

class menu:
    def __init__(self,
                 window_handle,
                 open_file_func,
                 save_file_func,
                 import_step_func,
                 close_func,
                 select_behind_func,
                 create_material_func,
                 assign_material_func,
                 assign_port_func,
                 validate_simulation_func,
                 run_simulation_func):
        
        # File ====================================================
        file_menu = window_handle.menuBar().addMenu('File')

        open_button = QtWidgets.QAction('Open', window_handle)
        open_button.setShortcut('Ctrl+O')
        open_button.setIcon(QtGui.QIcon(open_icon))
        open_button.triggered.connect(open_file_func)
        file_menu.addAction(open_button)

        save_button = QtWidgets.QAction('Save', window_handle)
        save_button.setShortcut('Ctrl+S')
        save_button.setIcon(QtGui.QIcon(save_icon))
        save_button.triggered.connect(save_file_func)
        file_menu.addAction(save_button)

        import_button = QtWidgets.QAction('Import .step file', window_handle)
        import_button.setIcon(QtGui.QIcon(import_icon))        
        import_button.triggered.connect(import_step_func)
        file_menu.addAction(import_button)

        exit_button = QtWidgets.QAction('Exit', window_handle)
        exit_button.setIcon(QtGui.QIcon(close_icon))
        exit_button.setShortcut('Ctrl+Q')
        exit_button.triggered.connect(close_func)
        file_menu.addAction(exit_button)

        # Edit ====================================================
        self.edit_menu = window_handle.menuBar().addMenu('Edit')
        self.edit_menu.setEnabled(False)
        
        select_behind_button = QtWidgets.QAction('Select Behind', window_handle)
        select_behind_button.setIcon(QtGui.QIcon(select_behind_icon))
        select_behind_button.setShortcut('b')
        select_behind_button.triggered.connect(select_behind_func)
        self.edit_menu.addAction(select_behind_button)        

        # Create ====================================================
        self.create_menu = window_handle.menuBar().addMenu('Create')

        create_material_button = QtWidgets.QAction('Material', window_handle)
        create_material_button.triggered.connect(create_material_func)
        create_material_button.setIcon(QtGui.QIcon(materials_icon))
        self.create_menu.addAction(create_material_button)

        # Assign ====================================================
        self.assign_menu = window_handle.menuBar().addMenu('Assign')
        self.assign_menu.setEnabled(False)

        self.assign_material_button = QtWidgets.QAction('Material', window_handle)
        self.assign_material_button.triggered.connect(assign_material_func)
        self.assign_material_button.setIcon(QtGui.QIcon(materials_icon))
        self.assign_material_button.setEnabled(False)
        self.assign_menu.addAction(self.assign_material_button)

        self.assign_port_button = QtWidgets.QAction('Port', window_handle)
        self.assign_port_button.triggered.connect(assign_port_func)
        self.assign_port_button.setIcon(QtGui.QIcon(ports_icon))
        self.assign_port_button.setEnabled(False)
        self.assign_menu.addAction(self.assign_port_button)

        # Simulation ====================================================
        self.simulation_menu = window_handle.menuBar().addMenu('Simulation')
        self.simulation_menu.setEnabled(True)

        self.validate_button = QtWidgets.QAction('Validate', window_handle)
        self.validate_button.triggered.connect(validate_simulation_func)
        self.validate_button.setIcon(QtGui.QIcon(validate_icon))
        self.simulation_menu.addAction(self.validate_button)

        self.run_button = QtWidgets.QAction('Run', window_handle)
        self.run_button.triggered.connect(run_simulation_func)
        self.run_button.setIcon(QtGui.QIcon(run_icon))
        self.simulation_menu.addAction(self.run_button)
        
    def enable_edit(self):
        self.edit_menu.setEnabled(True)
        self.assign_menu.setEnabled(True)

    def enable_port_assignment(self):
        self.assign_material_button.setEnabled(False)
        self.assign_port_button.setEnabled(True)

    def enable_material_assignment(self):
        self.assign_material_button.setEnabled(True)
        self.assign_port_button.setEnabled(False)

    def disable_assignment(self):
        self.assign_material_button.setEnabled(False)
        self.assign_port_button.setEnabled(False)

class DeselectableTreeWidget(QtWidgets.QTreeWidget):
    def __init__(self, deselect_callback):
        QtWidgets.QTreeWidget.__init__(self)
        self.callback = deselect_callback

    def mousePressEvent(self, event):
        self.clearSelection()
        self.callback()
        QtWidgets.QTreeWidget.mousePressEvent(self, event)

class model_tree:
    def __init__(self, deselected_func, selected_func):
        self.tree = DeselectableTreeWidget(deselected_func)
        self.tree.setColumnCount(2)
        self.tree.setHeaderHidden(True)
        self.tree.setColumnWidth(0, 150)

        self.tree_solids = QtWidgets.QTreeWidgetItem(["Solids"])
        self.tree_ports = QtWidgets.QTreeWidgetItem(["Ports"])
        self.tree_materials = QtWidgets.QTreeWidgetItem(["Materials"])
        self.tree_setup = QtWidgets.QTreeWidgetItem(["Setup"])
        self.tree_results = QtWidgets.QTreeWidgetItem(["Results"])
        self.tree.insertTopLevelItems(0, [
            self.tree_solids,
            self.tree_ports,
            self.tree_materials,
            self.tree_setup,
            self.tree_results])        

        self.tree_solids.setIcon(0, QtGui.QIcon(solids_icon))
        self.tree_ports.setIcon(0, QtGui.QIcon(ports_icon))
        self.tree_materials.setIcon(0, QtGui.QIcon(materials_icon))
        self.tree_setup.setIcon(0, QtGui.QIcon(setup_icon))
        self.tree_results.setIcon(0, QtGui.QIcon(results_icon))

        self.tree.itemClicked.connect(selected_func)

    def clear_solids(self):
        for i in reversed(range(self.tree_solids.childCount())):
            self.tree_solids.removeChild(self.tree_solids.child(i))

    def set_solids(self, ids):
        self.clear_solids()
        widget_items = []
        for id in ids:
            widget_items.append(QtWidgets.QTreeWidgetItem([id]))
        self.tree_solids.insertChildren(0, widget_items)

    def clear_materials(self):
        for i in reversed(range(self.tree_materials.childCount())):
            self.tree_materials.removeChild(self.tree_materials.child(i))

    def set_materials(self, materials):
        self.clear_materials()
        widget_items = []
        for mat in materials:
            widget_items.append(QtWidgets.QTreeWidgetItem([mat]))
        self.tree_materials.insertChildren(0, widget_items)

    def clear_ports(self):
        for i in reversed(range(self.tree_ports.childCount())):
            self.tree_ports.removeChild(self.tree_ports.child(i))

    def set_number_of_ports(self, num):
        self.clear_ports()
        widget_items = []
        for i in range(num):
            widget_items.append(QtWidgets.QTreeWidgetItem(["Port_" + str(i)]))
        self.tree_ports.insertChildren(0, widget_items)

    def clear_results(self):
        for i in reversed(range(self.tree_results.childCount())):
            self.tree_results.removeChild(self.tree_results.child(i))

    def set_results(self, results):
        self.clear_results()
        widget_items = []
        for set in results:
            widget_items.append(QtWidgets.QTreeWidgetItem([set]))
        self.tree_results.insertChildren(0, widget_items)

    def widget_handle(self):
        return self.tree
    
    def get_solid_index(self, it):
        return self.tree_solids.indexOfChild(it)
    
    def get_material_index(self, it):
        return self.tree_materials.indexOfChild(it)
    
    def get_port_index(self, it):
        return self.tree_ports.indexOfChild(it)
    
    def get_result_index(self, it):
        return self.tree_results.indexOfChild(it)
    
    def is_setup(self, it):
        return self.tree_setup is it
    
class value_table:
    def __init__(self, values):
        self.table = QtWidgets.QTableWidget()
        self.table.setRowCount(len(values))
        self.table.setColumnCount(2)
        self.table.horizontalHeader().setStretchLastSection(True)  
        self.table.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.table.horizontalHeader().setVisible(False)
        self.table.verticalHeader().setVisible(False)
        self.values = values

        i = 0
        for key, value in values.items():
            key_item = QtWidgets.QTableWidgetItem(str(key))
            val_item = QtWidgets.QTableWidgetItem(str(value))
            if isinstance(value, dict):
                val_item = QtWidgets.QTableWidgetItem(str(value["selected"]))

            key_item.setFlags(key_item.flags() ^ QtCore.Qt.ItemIsEditable)
            val_item.setFlags(val_item.flags() ^ QtCore.Qt.ItemIsEditable)
            self.table.setItem(i, 0, key_item)
            self.table.setItem(i, 1, val_item)
            i+=1

    def widget_handle(self):
        return self.table
    
class value_table_with_edit_and_delete_button():
    def __init__(self, values, edit_function, delete_function):
        self.widget = QtWidgets.QWidget()
        self.v_layout = QtWidgets.QVBoxLayout()        
        self.table = value_table(values)
        h_layout = QtWidgets.QHBoxLayout()
        
        edit_button = QtWidgets.QPushButton("Edit")
        delete_button = QtWidgets.QPushButton("Delete")

        if edit_function is not None:
            edit_button.clicked.connect(edit_function)
            h_layout.addWidget(edit_button)

        if delete_function is not None:
            delete_button.clicked.connect(delete_function)       
            h_layout.addWidget(delete_button)

        self.v_layout.addWidget(self.table.widget_handle())
        self.v_layout.addLayout(h_layout)
        self.widget.setLayout(self.v_layout)

    def widget_handle(self):
        return self.widget

class editable_value_table:
    def __init__(self, values):
        self.table = QtWidgets.QTableWidget()
        self.table.setRowCount(len(values))
        self.table.setColumnCount(2)
        self.table.horizontalHeader().setStretchLastSection(True)  
        self.table.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)
        self.table.horizontalHeader().setVisible(False)
        self.table.verticalHeader().setVisible(False)
        self.values = values

        i = 0
        for key, value in values.items():
            key_item = QtWidgets.QTableWidgetItem(str(key))
            key_item.setFlags(key_item.flags() ^ QtCore.Qt.ItemIsEditable)
            self.table.setItem(i, 0, key_item)

            if (isinstance(value, float)):
                val_item = QtWidgets.QLineEdit()
                val_item.setValidator(QtGui.QDoubleValidator())
                val_item.setText(str(value))
                self.table.setCellWidget(i, 1, val_item)

            elif isinstance(value, bool):
                val_item = QtWidgets.QComboBox()
                val_item.addItem("True")
                val_item.addItem("False")
                if value is True:
                    val_item.setCurrentIndex(0)
                else:
                    val_item.setCurrentIndex(1)
                self.table.setCellWidget(i, 1, val_item)

            elif isinstance(value, dict):
                val_item = QtWidgets.QComboBox()
                for item in value["options"]:
                    val_item.addItem(item)
                val_item.setCurrentIndex(value["options"].index(value["selected"]))
                self.table.setCellWidget(i, 1, val_item)

            elif isinstance(value, str):
                val_item = QtWidgets.QLineEdit()
                val_item.setText(str(value))
                self.table.setCellWidget(i, 1, val_item)

            elif isinstance(value, int):
                val_item = QtWidgets.QLineEdit()
                val_item.setValidator(QtGui.QIntValidator())
                val_item.setText(str(value))
                self.table.setCellWidget(i, 1, val_item)
            i+=1

    def widget_handle(self):
        return self.table
    
    def get_result(self):
        i = 0
        for key, value in self.values.items():
            if (isinstance(value, float)):
                self.values[key] = float(self.table.cellWidget(i, 1).text())
            elif (isinstance(value, bool)):
                text = self.table.cellWidget(i, 1).currentText()
                if text == "True":
                    self.values[key] = True
                else:
                    self.values[key] = False
            elif isinstance(value, dict):
                self.values[key]["selected"] = self.table.cellWidget(i, 1).currentText()
            elif isinstance(value, str):
                self.values[key] = str(self.table.cellWidget(i, 1).text())
            elif isinstance(value, int):
                self.values[key] = int(self.table.cellWidget(i, 1).text())
            i+=1
        return self.values

class table_create_dialog(QtWidgets.QDialog):
    def __init__(self, default_vals, title, parent=None):
        super().__init__(parent)

        self.setWindowTitle(title)

        QBtn = QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel

        self.buttonBox = QtWidgets.QDialogButtonBox(QBtn)
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)

        self.layout = QtWidgets.QVBoxLayout()
        
        self.table = editable_value_table(default_vals)
        self.layout.addWidget(self.table.widget_handle())
        self.layout.addWidget(self.buttonBox)
        self.setLayout(self.layout)

    def get_result(self):
        return self.table.get_result()

class warning_dialog(QtWidgets.QDialog):
    def __init__(self, title, message, parent=None):
        super().__init__(parent)

        self.setWindowTitle(title)
        self.setWindowIcon(QtGui.QIcon(warning_icon))

        QBtn = QtWidgets.QDialogButtonBox.Ok

        self.buttonBox = QtWidgets.QDialogButtonBox(QBtn)
        self.buttonBox.accepted.connect(self.accept)

        self.layout = QtWidgets.QVBoxLayout()
        message = QtWidgets.QLabel(message)
        self.layout.addWidget(message)
        self.layout.addWidget(self.buttonBox)
        self.setLayout(self.layout)

class list_select_dialog(QtWidgets.QDialog):
    def __init__(self, title, things, parent=None):
        super().__init__(parent)

        self.setWindowTitle(title)

        QBtn = QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel

        self.buttonBox = QtWidgets.QDialogButtonBox(QBtn)
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)        

        self.things = QtWidgets.QComboBox()
        for thing in things:
            self.things.addItem(thing)
        
        self.layout = QtWidgets.QVBoxLayout()
        self.layout.addWidget(self.things)
        self.layout.addWidget(self.buttonBox)
        self.setLayout(self.layout)

    def get_result(self):
        return self.things.currentText()
    
class console_box():
    def __init__(self):
        self.box = QtWidgets.QPlainTextEdit()
        self.box.setReadOnly(True)
        self.box.setFont(QtGui.QFont("Consolas", 10))
    
    def print_text(self, text):
        self.box.moveCursor(QtGui.QTextCursor.MoveOperation.End)
        self.box.insertPlainText(text)

    def widget_handle(self):
        return self.box
    
class MplCanvas(FigureCanvasQTAgg):
    def __init__(self, parent=None):
        self.fig, self.axes = plt.subplots()
        super(MplCanvas, self).__init__(self.fig)

class MplCanvasPolar(FigureCanvasQTAgg):
    def __init__(self, parent=None):
        self.fig, self.axes = plt.subplots(subplot_kw={'projection': 'polar'})
        super(MplCanvasPolar, self).__init__(self.fig)