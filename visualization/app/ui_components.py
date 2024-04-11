from qtpy import QtWidgets, QtCore

class menu:
    def __init__(self,
                 window_handle,
                 open_file_func,
                 save_file_func,
                 import_step_func,
                 close_func,
                 select_behind_func,
                 create_material_func):
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

        self.edit_menu = window_handle.menuBar().addMenu('Edit')
        
        select_behind_button = QtWidgets.QAction('Select Behind', window_handle)
        select_behind_button.setShortcut('b')
        select_behind_button.triggered.connect(select_behind_func)
        self.edit_menu.addAction(select_behind_button)
        self.edit_menu.setEnabled(False)

        self.create_menu = window_handle.menuBar().addMenu('Create')

        create_material_button = QtWidgets.QAction('Material', window_handle)
        create_material_button.triggered.connect(create_material_func)
        self.create_menu.addAction(create_material_button)
        

    def enable_edit(self):
        self.edit_menu.setEnabled(True)

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

    def widget_handle(self):
        return self.tree
    
    def is_solid_selection(self, it):
        return (it not in [self.tree_solids, self.tree_ports, self.tree_materials])
    
class volume_widget:
    def __init__(self):
        self.layout = QtWidgets.QGridLayout()

        material_label = QtWidgets.QLabel("Material")
        self.material = QtWidgets.QComboBox()

        self.layout.addWidget(material_label, 0, 0)
        self.layout.addWidget(self.material, 0, 1)

        self.container = QtWidgets.QWidget()
        self.container.setLayout(self.layout)

    def widget_handle(self):
        return self.container
    
    def set_materials(self, materials):
        self.material.clear()
        for mat in materials:
            self.material.addItem(mat)

class value_table:
    def __init__(self, values):
        self.table = QtWidgets.QTableWidget()
        self.table.setRowCount(len(values))
        self.table.setColumnCount(2)
        self.table.horizontalHeader().setStretchLastSection(True)  
        self.table.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)

        i = 0
        for key, value in values.items():
            self.table.setItem(i, 0, QtWidgets.QTableWidgetItem(key))

            val_item = QtWidgets.QTableWidgetItem()
            val_item.setData(QtCore.Qt.EditRole, value)
            self.table.setItem(i, 1, val_item)
            i+=1

    def widget_handle(self):
        return self.table


class material_dialog(QtWidgets.QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.setWindowTitle("Create Material")

        QBtn = QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel

        self.buttonBox = QtWidgets.QDialogButtonBox(QBtn)
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)

        self.layout = QtWidgets.QVBoxLayout()

        default_vals = {
            "ep": 1.0,
            "mu": 1.0,
            "tand": 0.00
        }
        
        self.table = value_table(default_vals)
        self.layout.addWidget(self.table.widget_handle())
        self.layout.addWidget(self.buttonBox)
        self.setLayout(self.layout)


        


