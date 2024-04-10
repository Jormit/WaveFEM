from qtpy import QtWidgets

class menu:
    def __init__(self, window_handle, open_file_func, save_file_func, import_step_func, close_func, select_behind_func):
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
        

    def enable_edit(self):
        self.edit_menu.setEnabled(True)