from qtpy import QtWidgets, QtCore, QtGui

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
                 assign_port_func):
        
        # File ====================================================
        file_menu = window_handle.menuBar().addMenu('File')

        open_button = QtWidgets.QAction('Open', window_handle)
        open_button.setShortcut('Ctrl+O')
        open_button.triggered.connect(open_file_func)
        file_menu.addAction(open_button)

        save_button = QtWidgets.QAction('Save', window_handle)
        save_button.setShortcut('Ctrl+S')
        save_button.triggered.connect(save_file_func)
        file_menu.addAction(save_button)

        import_button = QtWidgets.QAction('Import .step file', window_handle)
        import_button.triggered.connect(import_step_func)
        file_menu.addAction(import_button)

        exit_button = QtWidgets.QAction('Exit', window_handle)
        exit_button.setShortcut('Ctrl+Q')
        exit_button.triggered.connect(close_func)
        file_menu.addAction(exit_button)

        # Edit ====================================================
        self.edit_menu = window_handle.menuBar().addMenu('Edit')
        self.edit_menu.setEnabled(False)
        
        select_behind_button = QtWidgets.QAction('Select Behind', window_handle)
        select_behind_button.setShortcut('b')
        select_behind_button.triggered.connect(select_behind_func)
        self.edit_menu.addAction(select_behind_button)        

        # Create ====================================================
        self.create_menu = window_handle.menuBar().addMenu('Create')

        create_material_button = QtWidgets.QAction('Material', window_handle)
        create_material_button.triggered.connect(create_material_func)
        self.create_menu.addAction(create_material_button)

        # Assign ====================================================
        self.assign_menu = window_handle.menuBar().addMenu('Assign')
        self.assign_menu.setEnabled(False)

        assign_material_button = QtWidgets.QAction('Material', window_handle)
        assign_material_button.triggered.connect(assign_material_func)
        self.assign_menu.addAction(assign_material_button)

        port_material_button = QtWidgets.QAction('Port', window_handle)
        port_material_button.triggered.connect(assign_port_func)
        self.assign_menu.addAction(port_material_button)
        

    def enable_edit(self):
        self.edit_menu.setEnabled(True)
        self.assign_menu.setEnabled(True)

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
        self.tree.setColumnCount(1)
        self.tree.setHeaderHidden(True)
        self.tree_solids = QtWidgets.QTreeWidgetItem(["Solids"])
        self.tree_ports = QtWidgets.QTreeWidgetItem(["Ports"])
        self.tree_materials = QtWidgets.QTreeWidgetItem(["Materials"])
        self.tree.insertTopLevelItems(0, [self.tree_solids, self.tree_ports, self.tree_materials])
        self.tree.itemClicked.connect(selected_func)

    def set_solids(self, ids):
        widget_items = []
        for id in ids:
            widget_items.append(QtWidgets.QTreeWidgetItem(["Body"+str(id)]))
        self.tree_solids.insertChildren(0, widget_items)

    def add_material(self, materials):
        widget_items = []
        for mat in materials:
            widget_items.append(QtWidgets.QTreeWidgetItem([str(mat['name'])]))
        self.tree_materials.insertChildren(0, widget_items)


    def widget_handle(self):
        return self.tree
    
    def is_solid_selection(self, it):
        return (it.parent() is self.tree_solids)
    
    def is_material_selection(self, it):
        return (it.parent() is self.tree_materials)

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
            self.table.setItem(i, 0, QtWidgets.QTableWidgetItem(key))

            if (isinstance(value, float)):
                val_item = QtWidgets.QLineEdit()
                val_item.setValidator(QtGui.QDoubleValidator())
                val_item.setText(str(value))
                self.table.setCellWidget(i, 1, val_item)
            elif isinstance(value, bool):
                val_item = QtWidgets.QComboBox()
                val_item.addItem("True")
                val_item.addItem("False")
                if val_item is True:
                    val_item.setCurrentIndex(0)
                else:
                    val_item.setCurrentIndex(1)
                self.table.setCellWidget(i, 1, val_item)
            elif isinstance(value, str):
                val_item = QtWidgets.QLineEdit()
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
            if (isinstance(value, bool)):
                text = self.table.cellWidget(i, 1).currentText()
                if text == "True":
                    self.values[key] = True
                else:
                    self.values[key] = False
            elif isinstance(value, str):
                self.values[key] = str(self.table.cellWidget(i, 1).text())
            i+=1
        return self.values


class material_create_dialog(QtWidgets.QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.setWindowTitle("Create Material")

        QBtn = QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel

        self.buttonBox = QtWidgets.QDialogButtonBox(QBtn)
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)

        self.layout = QtWidgets.QVBoxLayout()

        default_vals = {
            "name": "Material",
            "ep": 1.0,
            "mu": 1.0,
            "tand": 0.00,
            "PEC": False
        }
        
        self.table = value_table(default_vals)
        self.layout.addWidget(self.table.widget_handle())
        self.layout.addWidget(self.buttonBox)
        self.setLayout(self.layout)

    def get_result(self):
        return self.table.get_result()

class warning_dialog(QtWidgets.QDialog):
    def __init__(self, title, message, parent=None):
        super().__init__(parent)

        self.setWindowTitle(title)

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

        


