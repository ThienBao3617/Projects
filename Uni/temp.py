import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QTextEdit, QAction, QFileDialog, QMessageBox


class TextEditor(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.textEdit = QTextEdit()
        self.setCentralWidget(self.textEdit)

        self.createMenuBar()
        self.setWindowTitle("Simple Text Editor")
        self.setGeometry(100, 100, 800, 600)

    def createMenuBar(self):
        mainMenu = self.menuBar()

        # File menu
        fileMenu = mainMenu.addMenu("File")

        openAction = QAction("Open", self)
        openAction.triggered.connect(self.openFile)
        fileMenu.addAction(openAction)

        saveAction = QAction("Save", self)
        saveAction.triggered.connect(self.saveFile)
        fileMenu.addAction(saveAction)

        exitAction = QAction("Exit", self)
        exitAction.triggered.connect(self.close)
        fileMenu.addAction(exitAction)

    def openFile(self):
        filePath, _ = QFileDialog.getOpenFileName(self, "Open File", "", "Text Files (*.txt)")
        if filePath:
            try:
                with open(filePath, "r") as file:
                    content = file.read()
                    self.textEdit.setPlainText(content)
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Failed to open file: {str(e)}")

    def saveFile(self):
        filePath, _ = QFileDialog.getSaveFileName(self, "Save File", "", "Text Files (*.txt)")
        if filePath:
            try:
                with open(filePath, "w") as file:
                    content = self.textEdit.toPlainText()
                    file.write(content)
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Failed to save file: {str(e)}")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    editor = TextEditor()
    editor.show()
    sys.exit(app.exec_())
