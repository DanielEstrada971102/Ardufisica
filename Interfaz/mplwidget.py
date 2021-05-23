# ------------------------------------------------------
# -------------------- mplwidget.py --------------------
# ------------------------------------------------------
from PyQt5.QtWidgets import QVBoxLayout, QWidget

from matplotlib.backends.backend_qt5agg import FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure

    
class MplWidget(QWidget):
    
    def __init__(self, parent = None):

        QWidget.__init__(self, parent)
        vertical_layout = QVBoxLayout()  
        
        self.canvas = FigureCanvas(Figure())
        vertical_layout.addWidget(self.canvas)        
        self.toolbar = NavigationToolbar(self.canvas, parent)
        vertical_layout.addWidget(self.toolbar)
        
        self.canvas.axes = self.canvas.figure.add_subplot(111)
        
        self.setLayout(vertical_layout)