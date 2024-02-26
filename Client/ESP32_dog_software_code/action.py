#!/usr/bin/python3
# -*- coding:utf-8 -*-
# @Time: 2022/05/26
# @Author: 林镇涛
# @Company: Freenove

import sys
import socket
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5 import QtWidgets

from Command import COMMAND
from ui_dance import Ui_form_dance

class ESP32_ACTION(QtWidgets.QWidget, Ui_form_dance):
    def __init__(self, client):
        super(ESP32_ACTION, self).__init__()
        self.setupUi(self)
        self.client = client
        self.cmd = COMMAND()
        # 添加界面的logo
        self.setWindowIcon(QIcon('./image/Freenove.ico'))  # 添加窗口logo

        self.set_button_picture()
        self.connect()

    #信号关联槽函数
    def connect(self):
        self.button_dance_dancing.clicked.connect(lambda: self.send_action_command(self.button_dance_dancing))
        self.button_dance_push_up.clicked.connect(lambda: self.send_action_command(self.button_dance_push_up))
        self.button_dance_say_hello.clicked.connect(lambda: self.send_action_command(self.button_dance_say_hello))
        self.button_dance_sit_down.clicked.connect(lambda: self.send_action_command(self.button_dance_sit_down))
        self.button_dance_stretch_self.clicked.connect(lambda: self.send_action_command(self.button_dance_stretch_self))
        self.button_dance_turn_around.clicked.connect(lambda: self.send_action_command(self.button_dance_turn_around))
        self.button_dance_picture_1.clicked.connect(lambda: self.send_action_command(self.button_dance_picture_1))
        self.button_dance_picture_2.clicked.connect(lambda: self.send_action_command(self.button_dance_picture_2))
        self.button_dance_picture_3.clicked.connect(lambda: self.send_action_command(self.button_dance_picture_3))
        self.button_dance_picture_4.clicked.connect(lambda: self.send_action_command(self.button_dance_picture_4))
        self.button_dance_picture_5.clicked.connect(lambda: self.send_action_command(self.button_dance_picture_5))
        self.button_dance_picture_6.clicked.connect(lambda: self.send_action_command(self.button_dance_picture_6))

    #设置按键背景图片
    def set_button_picture(self):
        img_path = './image/ico_say_hello.png'
        self.button_dance_picture_1.setStyleSheet(
            "QPushButton{\n" "background-image: url(\"%s\");\n" "}" % img_path)
        img_path = './image/ico_push_up.png'
        self.button_dance_picture_2.setStyleSheet(
            "QPushButton{\n" "background-image: url(\"%s\");\n" "}" % img_path)
        img_path = './image/ico_stretch_self.png'
        self.button_dance_picture_3.setStyleSheet(
            "QPushButton{\n" "background-image: url(\"%s\");\n" "}" % img_path)
        img_path = './image/ico_turn_around.png'
        self.button_dance_picture_4.setStyleSheet(
            "QPushButton{\n" "background-image: url(\"%s\");\n" "}" % img_path)
        img_path = './image/ico_sit_down.png'
        self.button_dance_picture_5.setStyleSheet(
            "QPushButton{\n" "background-image: url(\"%s\");\n" "}" % img_path)
        img_path = './image/ico_dancing.png'
        self.button_dance_picture_6.setStyleSheet(
            "QPushButton{\n" "background-image: url(\"%s\");\n" "}" % img_path)

    #按键事件处理函数
    def send_action_command(self, parameter):
        parameter_action = 0
        if parameter.objectName() == 'button_dance_say_hello' or parameter.objectName() == 'button_dance_picture_1':
            parameter_action = 0
        elif parameter.objectName() == 'button_dance_push_up' or parameter.objectName() == 'button_dance_picture_2':
            parameter_action = 1
        elif parameter.objectName() == 'button_dance_stretch_self' or parameter.objectName() == 'button_dance_picture_3':
            parameter_action = 2
        elif parameter.objectName() == 'button_dance_turn_around' or parameter.objectName() == 'button_dance_picture_4':
            parameter_action = 3
        elif parameter.objectName() == 'button_dance_sit_down' or parameter.objectName() == 'button_dance_picture_5':
            parameter_action = 4
        elif parameter.objectName() == 'button_dance_dancing' or parameter.objectName() == 'button_dance_picture_6':
            parameter_action = 5
        s = self.cmd.ACTION_DANCING + self.cmd.SPLIT + str(parameter_action) + self.cmd.SPLIT + self.cmd.ENTER
        self.client.send(s.encode('utf-8'))

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("192.168.1.139",5000))

    calibrationWindow = ESP32_ACTION(s)
    calibrationWindow.setWindowModality(Qt.ApplicationModal)
    calibrationWindow.show()

    #s.close()
    sys.exit(app.exec_())

