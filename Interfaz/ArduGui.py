from PyQt5.QtCore import QCoreApplication, QTimer, QRect
from PyQt5.QtWidgets import QApplication, QMainWindow, QDial
from Gui import Ui_ArdifisicaGui
from serial.tools.list_ports import comports
from PyQt5.QtGui import QPixmap
from time import sleep
import sys
import serial

class ArduGui(Ui_ArdifisicaGui):
    """docstring for ArduGui"""
    def __init__(self, mainWindow):
        super( ArduGui, self).__init__()
        self.setupUi(mainWindow)

        self.mainWindow = mainWindow

        self.redLedIcon = QPixmap(".images/led-red-on.png").scaled(20,20)
        self.greenLedIcon = QPixmap(".images/green-led-on.png").scaled(20,20)
        self.Led.setPixmap(self.redLedIcon)

        self.runStopButton.setStyleSheet('QPushButton {color: green;}')
        self.runStopButton_state = True

        self.serialport = None
        self.refreshCOMPorts()
        
        self.sensor = None
        self.command = ""
        self.freqs_GS = ["261", "277", "293", "311", "329", "349", "369", "391", "415", "440", "466", "493"]
        #self.xdata = around(linspace(0,360, 360),2)
        #self.posx = -len(self.xdata)
        self.ydata = []
        self.Timer = QTimer(self.centralwidget)
        self.Timer.timeout.connect(lambda : 0)


        self.comboBox_ports.currentIndexChanged.connect(self.portSelection)
        self.runStopButton.clicked.connect(self.run_stop)
        self.runStopButton_2.clicked.connect(self.run_stop)
        self.saveButton.clicked.connect(lambda: 0)
        self.dialEncoder.valueChanged.connect(self.Encodere_function)
        #self.grafica.clicked.connect(lambda: 0)

        self.aireButton.clicked.connect(self.airSensor)
        self.tacometroButton.clicked.connect(self.tacometer)
        self.genSonbutton.clicked.connect(self.gen_sound)
        self.microfonoButton.clicked.connect(self.microphone)
        self.tempIRbutton.clicked.connect(self.tempIRSensor)
        self.luzButton.clicked.connect(self.lightSensor)
        self.atomizadorButton.clicked.connect(self.atomizer)
        self.fotoRButton.clicked.connect(self.photoRSensor)
        self.termoKButton.clicked.connect(self.tempKSensor)
        self.colorButton.clicked.connect(self.colorSensor)
        self.servoButton.clicked.connect(self.servoM)
        self.ledRGBbutton.clicked.connect(self.ledRGB)
        self.hallButton.clicked.connect(self.hallSensor)



    def airSensor(self):
        self.sensor = "calidad"
        self.lcdMessage(self.sensor)


    def tacometer(self):
        self.sensor = "tacometro"
        self.lcdMessage(self.sensor)


    def gen_sound(self):
        self.sensor = "gen.sonido"
        self.lcdMessage(self.sensor)

        self.dialEncoder.setMinimum(0)
        self.dialEncoder.setMaximum(len(self.freqs_GS) - 1)
        self.labelEncoder.setText("0 Hz")


    def microphone(self):
        self.sensor = "microfono"
        self.lcdMessage(self.sensor) 
        self.dialEncoder.setMinimum(0)
        self.dialEncoder.setMaximum(0)
        self.labelEncoder.setText("")      


    def tempIRSensor(self):
        self.sensor = "temperaturaIR"
        self.lcdMessage(self.sensor)
        self.dialEncoder.setMinimum(0)
        self.dialEncoder.setMaximum(0)
        self.labelEncoder.setText("")


    def lightSensor(self):
        self.sensor = "luz"
        self.lcdMessage(self.sensor)
        self.dialEncoder.setMinimum(0)
        self.dialEncoder.setMaximum(0)
        self.labelEncoder.setText("")


    def atomizer(self):
        self.sensor = "atomizador"
        self.lcdMessage(self.sensor)
        self.dialEncoder.setMinimum(0)
        self.dialEncoder.setMaximum(1)
        self.labelEncoder.setText("off")


    def photoRSensor(self):
        self.sensor = "fotorresistencia"
        self.lcdMessage(self.sensor)
        self.dialEncoder.setMinimum(0)
        self.dialEncoder.setMaximum(0)
        self.labelEncoder.setText("")

    def tempKSensor(self):
        self.sensor = "temperaturaK"
        self.lcdMessage(self.sensor)
        self.dialEncoder.setMinimum(0)
        self.dialEncoder.setMaximum(0)
        self.labelEncoder.setText("")

    def colorSensor(self):
        self.sensor = "color"
        self.lcdMessage(self.sensor)


    def servoM(self):
        self.sensor = "servo"
        self.lcdMessage(self.sensor)


    def ledRGB(self):
        self.sensor = "ledRGB"
        self.lcdMessage(self.sensor)


    def hallSensor(self):
        self.sensor = "hall" 
        self.lcdMessage(self.sensor)
        self.dialEncoder.setMinimum(0)
        self.dialEncoder.setMaximum(0)
        self.labelEncoder.setText("")

    def lcdMessage(self, line1, line2 = " "):
        _translate = QCoreApplication.translate
        start_line = "<html><head/><body><p><span style=\" font-size:18pt; font-weight:600; color:#ffffff;\">"
        end_line = "</span></p></body></html>"

        self.labelSensor.setText(_translate("ArdifisicaGui", start_line + line1 + start_line + line2 + end_line))


    def portSelection(self):
        selectItem = self.comboBox_ports.currentText()
        self.serialport = serial.Serial()
        self.serialport.baud = "115200"
        
        if selectItem == "refresh":
            self.refreshCOMPorts()

        elif selectItem == "---":
            self.Led.setPixmap(self.redLedIcon)
            self.serialport.close()

        else:
            try:
                self.serialport.port = selectItem
                self.serialport.open()
                self.Led.setPixmap(self.greenLedIcon)
            except:
                self.Led.setPixmap(self.redLedIcon)


    def refreshCOMPorts(self):
        ports = ["---", "refresh"]
        
        for i in comports():
            ports.append(i.device)

        self.comboBox_ports.clear()
        self.comboBox_ports.addItems(ports)


    def run_stop(self):
        if self.runStopButton_state:
            self.runStopButton.setStyleSheet('QPushButton {color: red;}')
            self.runStopButton_2.setStyleSheet('QPushButton {color: red;}')
            self.runStopButton.setText("STOP")
            self.runStopButton_2.setText("STOP")

            self.command = self.sensor
            self.Timer.start(200)

        else: 
            self.runStopButton.setStyleSheet('QPushButton {color: green;}')
            self.runStopButton_2.setStyleSheet('QPushButton {color: green;}')
            self.runStopButton.setText("RUN")
            self.runStopButton_2.setText("RUN")

            self.Timer.stop()
            self.command = "stop"

        self.runStopButton_state = not self.runStopButton_state 


    def Encodere_function(self):
        
        if self.sensor == "gen.sonido":
            self.command = self.freqs_GS[self.dialEncoder.value()]
            self.labelEncoder.setText(self.command + " Hz")
        
        elif self.sensor == "atomizador":
            if self.dialEncoder.value():  
                self.labelEncoder.setText("on")
            else:
                self.labelEncoder.setText("off")
        else: 
            pass



                

def main():
    app = QApplication(sys.argv)
    ArduWindow = QMainWindow()
    ui = ArduGui(ArduWindow)
    ArduWindow.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()