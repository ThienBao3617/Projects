import sys
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLabel, QLineEdit, QPushButton, QMessageBox
from create import buildDatabase


class UserManagementApp(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("User Management System")
        self.setGeometry(100, 100, 400, 200)
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        self.create_register_widgets()
        self.create_login_widgets()
        self.create_logout_widgets()  # Add logout widgets
        self.create_delete_widgets()  # Add delete widgets

        self.database = buildDatabase()

    def create_register_widgets(self):
        self.register_label = QLabel("Register New User:")
        self.layout.addWidget(self.register_label)

        self.name_entry = QLineEdit()
        self.name_entry.setPlaceholderText("Name")
        self.layout.addWidget(self.name_entry)

        self.phone_entry = QLineEdit()
        self.phone_entry.setPlaceholderText("Phone Number")
        self.layout.addWidget(self.phone_entry)

        self.address_entry = QLineEdit()
        self.address_entry.setPlaceholderText("Address")
        self.layout.addWidget(self.address_entry)

        self.register_button = QPushButton("Register")
        self.register_button.clicked.connect(self.register_user)
        self.layout.addWidget(self.register_button)

    def create_login_widgets(self):
        self.login_label = QLabel("Login:")
        self.layout.addWidget(self.login_label)

        self.login_name_entry = QLineEdit()
        self.login_name_entry.setPlaceholderText("Name")
        self.layout.addWidget(self.login_name_entry)

        self.login_button = QPushButton("Login")
        self.login_button.clicked.connect(self.login_user)
        self.layout.addWidget(self.login_button)

    def create_logout_widgets(self):
        self.logout_button = QPushButton("Logout")
        self.logout_button.clicked.connect(self.logout_user)
        self.layout.addWidget(self.logout_button)

    def create_delete_widgets(self):
        self.delete_label = QLabel("Delete Account:")
        self.layout.addWidget(self.delete_label)

        self.delete_name_entry = QLineEdit()
        self.delete_name_entry.setPlaceholderText("Name")
        self.layout.addWidget(self.delete_name_entry)

        self.delete_button = QPushButton("Delete Account")
        self.delete_button.clicked.connect(self.delete_user)
        self.layout.addWidget(self.delete_button)

    def register_user(self):
        name = self.name_entry.text().strip()
        phone_number = self.phone_entry.text().strip()
        address_number = self.address_entry.text().strip()

        if name and phone_number:
            self.database.add_account(name, phone=phone_number, address=address_number)
            QMessageBox.information(self, "Registration", "User registered successfully!")
            self.name_entry.clear()
            self.phone_entry.clear()
            self.address_entry.clear()
        else:
            QMessageBox.warning(self, "Error", "Please enter the remain informations.")

    def login_user(self):
        name = self.login_name_entry.text().strip()

        # Implement login logic here (e.g., check if user exists in database)
        # For simplicity, just show a message box
        QMessageBox.information(self, "Login", f"Welcome, {name}!")

    def logout_user(self):
        # Implement logout logic here (if needed)
        QMessageBox.information(self, "Logout", "Logged out successfully!")

    def delete_user(self):
        name = self.delete_name_entry.text().strip()

        if name:
            self.database.rem_account(name)
            QMessageBox.information(self, "Account Deletion", "User account deleted successfully!")
            self.delete_name_entry.clear()
        else:
            QMessageBox.warning(self, "Error", "Please enter the name to delete.")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = UserManagementApp()
    window.show()
    sys.exit(app.exec_())
