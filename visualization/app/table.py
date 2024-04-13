from qtpy import QtCore, QtGui, QtWidgets
import sys

app = QtWidgets.QApplication([])
table = QtWidgets.QTableWidget(2,2)

tableItem = QtWidgets.QLineEdit()
tableItem.setValidator(QtGui.QDoubleValidator())
tableItem.setText( "Testing" )
table.setCellWidget(0, 0, tableItem )

comboBox = QtWidgets.QComboBox()
table.setCellWidget(1,1, comboBox)

table.show()
sys.exit(app.exec_())