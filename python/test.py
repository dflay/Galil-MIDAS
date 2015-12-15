# importa QtGui
from PySide.QtGui import *
from subprocess import call

# crea una applicazione Qt
app = QApplication([])

# crea una finestra
window = QWidget()
#crea un layout
layout = QVBoxLayout()
layout = QVBoxLayout()
#crea 2 bottoni
button1 = QPushButton("Axis A")
textedit1 = QTextEdit('')
button2 = QPushButton("Axis B")
textedit2 = QTextEdit('')
button3 = QPushButton("Axis C")
textedit3 = QTextEdit('')
#aggiunge i bottoni al layout
layout.addWidget(button1,0,200)
layout.addWidget(textedit1,200,200)
layout.addWidget(button2,0,200)
layout.addWidget(textedit2,200,200)
layout.addWidget(button3,0,200)
layout.addWidget(textedit3,200,200)

# imposta il titolo e la dimensione
window.resize(500, 300)
window.setWindowTitle('Fondamenti di Programmazione')
window.setLayout(layout)


button=QPushButton('Button', window)
def button_callback():
    call(["/home/matteo/./teapot"])

button.clicked.connect(button_callback)


# rende la finestra visibile
window.show()

# lancia l'interazione utente
app.exec_()
