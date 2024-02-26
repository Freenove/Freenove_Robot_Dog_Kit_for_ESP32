#!/usr/bin/python3
# -*- coding:utf-8 -*-
# @Time: 2022/05/26
# @Author: 林镇涛
# @Company: Freenove

import sys
import socket
from PyQt5.QtCore import *
import numpy as np
from PyQt5.QtGui import *
from PyQt5 import QtWidgets,QtCore

from Command import COMMAND
from ui_led import Ui_led

#QT调色板的对话框类
class ColorDialog(QtWidgets.QColorDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setOptions(self.options() | QtWidgets.QColorDialog.DontUseNativeDialog)
        for children in self.findChildren(QtWidgets.QWidget):
            classname = children.metaObject().className()
            if classname not in ("QColorPicker", "QColorLuminancePicker"):
                children.hide()

class ESP32_LED(QtWidgets.QWidget, Ui_led):
    def __init__(self, client):
        super(ESP32_LED, self).__init__()
        self.setupUi(self)
        # 添加界面的logo
        self.setWindowIcon(QIcon('./image/Freenove.ico'))  # 添加窗口logo
        self.client = client
        self.cmd = COMMAND()
        self.led_mode = 0
        self.led_brightness = 255

        self.cmd = COMMAND()
        lineEdit_limit_validator = QRegExpValidator(QRegExp('^?([0,1]?\d?\d|2[0-4]\d|25[0-5])$'))
        self.lineEdit_led_color_r.setValidator(lineEdit_limit_validator)
        self.lineEdit_led_color_g.setValidator(lineEdit_limit_validator)
        self.lineEdit_led_color_b.setValidator(lineEdit_limit_validator)
        self.lineEdit_led_brightness.setValidator(lineEdit_limit_validator)

        r = int(self.lineEdit_led_color_r.text())
        g = int(self.lineEdit_led_color_g.text())
        b = int(self.lineEdit_led_color_b.text())
        self.color = [r, g, b]

        self.connect()
        self.rgb255_brightness_transition()

    def connect(self):
        self.verticalSlider_led_brightness.valueChanged.connect(self.led_brightnessness_show)
        self.checkBox_led_mode_off.clicked.connect(lambda: self.led_mode_select(self.checkBox_led_mode_off))
        self.checkBox_led_mode_rgb.clicked.connect(lambda: self.led_mode_select(self.checkBox_led_mode_rgb))
        self.checkBox_led_mode_following.clicked.connect(lambda: self.led_mode_select(self.checkBox_led_mode_following))
        self.checkBox_led_mode_blink.clicked.connect(lambda: self.led_mode_select(self.checkBox_led_mode_blink))
        self.checkBox_led_mode_breathing.clicked.connect(lambda: self.led_mode_select(self.checkBox_led_mode_breathing))
        self.checkBox_led_mode_rainbow.clicked.connect(lambda: self.led_mode_select(self.checkBox_led_mode_rainbow))
        self.lineEdit_led_color_r.textChanged.connect(self.lineEdit_led_text_change)
        self.lineEdit_led_color_g.textChanged.connect(self.lineEdit_led_text_change)
        self.lineEdit_led_color_b.textChanged.connect(self.lineEdit_led_text_change)
        self.lineEdit_led_brightness.textChanged.connect(self.lineEdit_led_text_change)
        self.button_led_update_color.clicked.connect(self.update_lineedit_color)

        self.colordialog = ColorDialog()
        self.colordialog.currentColorChanged.connect(self.led_color_disk_show)
        lay = QtWidgets.QVBoxLayout(self.widget_led_color_disk)
        lay.addWidget(self.colordialog, alignment=QtCore.Qt.AlignCenter)

    #rgbhex转rgb255
    def rgbhex_to_rgb255(self, rgbhex: str) -> np.array:
        if rgbhex[0] == '#':
            rgbhex = rgbhex[1:]
        r = int(rgbhex[0:2], 16)
        g = int(rgbhex[2:4], 16)
        b = int(rgbhex[4:6], 16)
        return np.array((r, g, b))

    #获取颜色数据：色盘原始颜色和亮度值进行计算后返回实际数据，同时互动显示在界面上
    def rgb255_brightness_transition(self):
        r = int(self.led_brightness * self.color[0] / 255)
        g = int(self.led_brightness * self.color[1] / 255)
        b = int(self.led_brightness * self.color[2] / 255)
        #根据颜色生成stylesheet
        label_led_color_disk_show = 'QLabel {background-color: rgb(' + str(r) +',' + str(g) +','+ str(b) + ');}'
        #设置界面颜色数值和显示选中颜色
        self.label_led_color_disk.setStyleSheet(str(label_led_color_disk_show))
        self.lineEdit_led_color_r.setText(str(r))
        self.lineEdit_led_color_g.setText(str(g))
        self.lineEdit_led_color_b.setText(str(b))
        return r,g,b

    #色盘数据获取并显示在颜色显示标签上，数据显示在数值标签上
    def led_color_disk_show(self, color):
        #获取色盘原始数据
        self.color= self.rgbhex_to_rgb255(color.name())
        #获取计算后颜色值
        r,g,b = self.rgb255_brightness_transition()
        #根据模式发送对应的指令
        s = self.cmd.ACTION_RGB + self.cmd.SPLIT + str(self.led_mode) + self.cmd.SPLIT + str(r) + self.cmd.SPLIT + str(g) + self.cmd.SPLIT + str(b) + self.cmd.SPLIT + self.cmd.ENTER
        if (self.led_mode!=0) and (self.led_mode!=5):
            self.client.send(s.encode('utf-8'))

    #led亮度滑条数值显示函数
    def led_brightnessness_show(self):
        #获取亮度值
        self.led_brightness = self.verticalSlider_led_brightness.value()
        #显示亮度值
        self.lineEdit_led_brightness.setText(str(self.led_brightness))
        #获取计算后颜色值
        r,g,b = self.rgb255_brightness_transition()
        # 根据模式发送对应的指令
        s = self.cmd.ACTION_RGB + self.cmd.SPLIT + str(self.led_mode) + self.cmd.SPLIT + str(r) + self.cmd.SPLIT + str(g) + self.cmd.SPLIT + str(b) + self.cmd.SPLIT + self.cmd.ENTER
        if (self.led_mode != 0) and (self.led_mode != 5):
            self.client.send(s.encode('utf-8'))

    #选择彩灯显示模式
    def led_mode_select(self, parameter):
        if parameter.objectName() == 'checkBox_led_mode_off':
            self.led_mode = 0
            self.checkBox_led_mode_rgb.setChecked(False)
            self.checkBox_led_mode_following.setChecked(False)
            self.checkBox_led_mode_blink.setChecked(False)
            self.checkBox_led_mode_breathing.setChecked(False)
            self.checkBox_led_mode_rainbow.setChecked(False)
        elif parameter.objectName() == 'checkBox_led_mode_rgb':
            if self.checkBox_led_mode_rgb.isChecked() == True:
                self.led_mode = 1
            elif self.checkBox_led_mode_rgb.isChecked() == False:
                self.led_mode = 0
            self.checkBox_led_mode_off.setChecked(False)
            self.checkBox_led_mode_following.setChecked(False)
            self.checkBox_led_mode_blink.setChecked(False)
            self.checkBox_led_mode_breathing.setChecked(False)
            self.checkBox_led_mode_rainbow.setChecked(False)
        elif parameter.objectName() == 'checkBox_led_mode_following':
            if self.checkBox_led_mode_following.isChecked() == True:
                self.led_mode = 2
            elif self.checkBox_led_mode_following.isChecked() == False:
                self.led_mode = 0
            self.checkBox_led_mode_off.setChecked(False)
            self.checkBox_led_mode_rgb.setChecked(False)
            self.checkBox_led_mode_blink.setChecked(False)
            self.checkBox_led_mode_breathing.setChecked(False)
            self.checkBox_led_mode_rainbow.setChecked(False)
        elif parameter.objectName() == 'checkBox_led_mode_blink':
            if self.checkBox_led_mode_blink.isChecked() == True:
                self.led_mode = 3
            elif self.checkBox_led_mode_blink.isChecked() == False:
                self.led_mode = 0
            self.checkBox_led_mode_off.setChecked(False)
            self.checkBox_led_mode_rgb.setChecked(False)
            self.checkBox_led_mode_following.setChecked(False)
            self.checkBox_led_mode_breathing.setChecked(False)
            self.checkBox_led_mode_rainbow.setChecked(False)
        elif parameter.objectName() == 'checkBox_led_mode_breathing':
            if self.checkBox_led_mode_breathing.isChecked() == True:
                self.led_mode = 4
            elif self.checkBox_led_mode_breathing.isChecked() == False:
                self.led_mode = 0
            self.checkBox_led_mode_off.setChecked(False)
            self.checkBox_led_mode_rgb.setChecked(False)
            self.checkBox_led_mode_following.setChecked(False)
            self.checkBox_led_mode_blink.setChecked(False)
            self.checkBox_led_mode_rainbow.setChecked(False)
        elif parameter.objectName() == 'checkBox_led_mode_rainbow':
            if self.checkBox_led_mode_rainbow.isChecked() == True:
                self.led_mode = 5
            elif self.checkBox_led_mode_rainbow.isChecked() == False:
                self.led_mode = 0
            self.checkBox_led_mode_off.setChecked(False)
            self.checkBox_led_mode_rgb.setChecked(False)
            self.checkBox_led_mode_following.setChecked(False)
            self.checkBox_led_mode_blink.setChecked(False)
            self.checkBox_led_mode_breathing.setChecked(False)
        #print(self.led_mode)

        r, g, b = self.rgb255_brightness_transition()
        # 根据模式发送对应的指令
        s = self.cmd.ACTION_RGB + self.cmd.SPLIT + str(self.led_mode) + self.cmd.SPLIT + str(r) + self.cmd.SPLIT + str(g) + self.cmd.SPLIT + str(b) + self.cmd.SPLIT + self.cmd.ENTER
        self.client.send(s.encode('utf-8'))

    #当用户手动修改文本值时，自动更新颜色
    def lineEdit_led_text_change(self):
        try:
            bright = int(self.lineEdit_led_brightness.text())
            r1 = int(self.lineEdit_led_color_r.text())
            g1 = int(self.lineEdit_led_color_g.text())
            b1 = int(self.lineEdit_led_color_b.text())
            r2 = int(bright * r1 / 255)
            g2 = int(bright * g1 / 255)
            b2 = int(bright * b1 / 255)
            label_led_color_disk_show = 'QLabel {background-color: rgb(' + str(r2) + ',' + str(g2) + ',' + str(b2) + ');}'
            self.label_led_color_disk.setStyleSheet(str(label_led_color_disk_show))
        except:
            pass

    #当用户手动修改文本值时，需要点击这个按键才能更新颜色数据
    def update_lineedit_color(self):
        try:
            self.color[0] = int(self.lineEdit_led_color_r.text())
            self.color[1] = int(self.lineEdit_led_color_g.text())
            self.color[2] = int(self.lineEdit_led_color_b.text())
        except:
            pass

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("192.168.1.80",5000))

    calibrationWindow = ESP32_LED(s)
    calibrationWindow.setWindowModality(Qt.ApplicationModal)
    calibrationWindow.show()

    #s.close()
    sys.exit(app.exec_())

