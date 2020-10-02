from PyQt5 import QtWidgets
from controller.gui.ui.login_ui import Ui_Login


class LoginForm(QtWidgets.QMainWindow):
    def __init__(self):
        super(LoginForm, self).__init__()

        self.ui = Ui_Login()
        self.ui.setupUi(self)
