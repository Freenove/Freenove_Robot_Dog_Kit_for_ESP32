#!/usr/bin/python3
# -*- coding:utf-8 -*-
# @Time: 2022/05/26
# @Author: 林镇涛
# @Company: Freenove

import sys
import socket
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5 import QtWidgets,QtCore

from Command import COMMAND
from ui_calibration import Ui_form_calibration

class ESP32_Calibration(QtWidgets.QWidget, Ui_form_calibration):
    def __init__(self, client):
        super(ESP32_Calibration, self).__init__()
        self.setupUi(self)
        # 添加界面的logo
        self.setWindowIcon(QIcon('./image/Freenove.ico'))  # 添加窗口logo
        self.client = client
        self.connect()
        self.cmd = COMMAND()
        self.calibration_targets_state = 0
        self.calibration_targets_leg = -1
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.refresh_image)
        self.label_picture.setPixmap(QPixmap('./image/esp32_calibration_leg.png'))


    def connect(self):
        self.button_calibration_installation.clicked.connect(lambda: self.calibration_config(self.button_calibration_installation))
        self.button_calibration_Verify.clicked.connect(lambda: self.calibration_config(self.button_calibration_Verify))
        self.button_calibration_Original.clicked.connect(lambda: self.calibration_config(self.button_calibration_Original))
        self.button_calibration_Submit.clicked.connect(lambda: self.calibration_config(self.button_calibration_Submit))
        self.button_calibration_x1.clicked.connect(lambda: self.calibration_cmd(self.button_calibration_x1))
        self.button_calibration_x2.clicked.connect(lambda: self.calibration_cmd(self.button_calibration_x2))
        self.button_calibration_y1.clicked.connect(lambda: self.calibration_cmd(self.button_calibration_y1))
        self.button_calibration_y2.clicked.connect(lambda: self.calibration_cmd(self.button_calibration_y2))
        self.button_calibration_z1.clicked.connect(lambda: self.calibration_cmd(self.button_calibration_z1))
        self.button_calibration_z2.clicked.connect(lambda: self.calibration_cmd(self.button_calibration_z2))
        self.checkBox_calibration_leg0.clicked.connect(lambda: self.calibration_target(self.checkBox_calibration_leg0))
        self.checkBox_calibration_leg1.clicked.connect(lambda: self.calibration_target(self.checkBox_calibration_leg1))
        self.checkBox_calibration_leg2.clicked.connect(lambda: self.calibration_target(self.checkBox_calibration_leg2))
        self.checkBox_calibration_leg3.clicked.connect(lambda: self.calibration_target(self.checkBox_calibration_leg3))

    #机器狗配置校准数据命令
    def calibration_config(self, parameter):
        if parameter.objectName() == 'button_calibration_installation':
            s = self.cmd.ACTION_INSTALLATION + self.cmd.SPLIT + '1' + self.cmd.SPLIT + self.cmd.ENTER
            self.client.send(s.encode('utf-8'))
        elif parameter.objectName() == 'button_calibration_Verify':
            s = self.cmd.ACTION_UP_DOWN + self.cmd.SPLIT + self.cmd.ENTER
            self.client.send(s.encode('utf-8'))
        elif parameter.objectName() == 'button_calibration_Original':
            s = self.cmd.ACTION_INSTALLATION + self.cmd.SPLIT + '2' + self.cmd.SPLIT + self.cmd.ENTER
            self.client.send(s.encode('utf-8'))
        elif parameter.objectName() == 'button_calibration_Submit':
            if self.calibration_targets_leg == 0:
                x = int(self.lineEdit_calibration_led0_x.text())
                y = int(self.lineEdit_calibration_led0_y.text())
                z = int(self.lineEdit_calibration_led0_z.text())
            elif self.calibration_targets_leg == 1:
                x = int(self.lineEdit_calibration_led1_x.text())
                y = int(self.lineEdit_calibration_led1_y.text())
                z = int(self.lineEdit_calibration_led1_z.text())
            elif self.calibration_targets_leg == 2:
                x = int(self.lineEdit_calibration_led2_x.text())
                y = int(self.lineEdit_calibration_led2_y.text())
                z = int(self.lineEdit_calibration_led2_z.text())
            elif self.calibration_targets_leg == 3:
                x = int(self.lineEdit_calibration_led3_x.text())
                y = int(self.lineEdit_calibration_led3_y.text())
                z = int(self.lineEdit_calibration_led3_z.text())
            if self.calibration_targets_leg != -1:
                s = self.cmd.ACTION_CALIBRATE + self.cmd.SPLIT + str(self.calibration_targets_leg) + self.cmd.SPLIT + '2' + self.cmd.SPLIT + \
                    str(x) + self.cmd.SPLIT + str(y) + self.cmd.SPLIT + str(z) + self.cmd.SPLIT + self.cmd.ENTER
                self.client.send(s.encode('utf-8'))

    # 机器狗调节校准命令
    def calibration_cmd(self, parameter):
        # 如果没有选中需要校准的腿，打印提示信息，并退出槽函数
        if self.calibration_targets_leg == -1:
            QMessageBox.information(self, "Freenove", "Please select the leg to configure.")
            return None
        # 如果已经选中需要校准的腿，操作x，y，z，同步显示在界面上，并发送指令
        else:
            # 获取x,y,z值
            if self.calibration_targets_leg == 0:
                x = int(self.lineEdit_calibration_led0_x.text())
                y = int(self.lineEdit_calibration_led0_y.text())
                z = int(self.lineEdit_calibration_led0_z.text())
            elif self.calibration_targets_leg == 1:
                x = int(self.lineEdit_calibration_led1_x.text())
                y = int(self.lineEdit_calibration_led1_y.text())
                z = int(self.lineEdit_calibration_led1_z.text())
            elif self.calibration_targets_leg == 2:
                x = int(self.lineEdit_calibration_led2_x.text())
                y = int(self.lineEdit_calibration_led2_y.text())
                z = int(self.lineEdit_calibration_led2_z.text())
            elif self.calibration_targets_leg == 3:
                x = int(self.lineEdit_calibration_led3_x.text())
                y = int(self.lineEdit_calibration_led3_y.text())
                z = int(self.lineEdit_calibration_led3_z.text())
            # 操作x,y,z值，并显示在界面上
            if parameter.objectName() == "button_calibration_x1":
                x = x - 1
                if self.calibration_targets_leg == 0:
                    self.lineEdit_calibration_led0_x.setText(str(x))
                elif self.calibration_targets_leg == 1:
                    self.lineEdit_calibration_led1_x.setText(str(x))
                elif self.calibration_targets_leg == 2:
                    self.lineEdit_calibration_led2_x.setText(str(x))
                elif self.calibration_targets_leg == 3:
                    self.lineEdit_calibration_led3_x.setText(str(x))
            elif parameter.objectName() == "button_calibration_x2":
                x = x + 1
                if self.calibration_targets_leg == 0:
                    self.lineEdit_calibration_led0_x.setText(str(x))
                elif self.calibration_targets_leg == 1:
                    self.lineEdit_calibration_led1_x.setText(str(x))
                elif self.calibration_targets_leg == 2:
                    self.lineEdit_calibration_led2_x.setText(str(x))
                elif self.calibration_targets_leg == 3:
                    self.lineEdit_calibration_led3_x.setText(str(x))
            elif parameter.objectName() == "button_calibration_y1":
                y = y - 1
                if self.calibration_targets_leg == 0:
                    self.lineEdit_calibration_led0_y.setText(str(y))
                elif self.calibration_targets_leg == 1:
                    self.lineEdit_calibration_led1_y.setText(str(y))
                elif self.calibration_targets_leg == 2:
                    self.lineEdit_calibration_led2_y.setText(str(y))
                elif self.calibration_targets_leg == 3:
                    self.lineEdit_calibration_led3_y.setText(str(y))
            elif parameter.objectName() == "button_calibration_y2":
                y = y + 1
                if self.calibration_targets_leg == 0:
                    self.lineEdit_calibration_led0_y.setText(str(y))
                elif self.calibration_targets_leg == 1:
                    self.lineEdit_calibration_led1_y.setText(str(y))
                elif self.calibration_targets_leg == 2:
                    self.lineEdit_calibration_led2_y.setText(str(y))
                elif self.calibration_targets_leg == 3:
                    self.lineEdit_calibration_led3_y.setText(str(y))
            elif parameter.objectName() == "button_calibration_z1":
                z = z - 1
                if self.calibration_targets_leg == 0:
                    self.lineEdit_calibration_led0_z.setText(str(z))
                elif self.calibration_targets_leg == 1:
                    self.lineEdit_calibration_led1_z.setText(str(z))
                elif self.calibration_targets_leg == 2:
                    self.lineEdit_calibration_led2_z.setText(str(z))
                elif self.calibration_targets_leg == 3:
                    self.lineEdit_calibration_led3_z.setText(str(z))
            elif parameter.objectName() == "button_calibration_z2":
                z = z + 1
                if self.calibration_targets_leg == 0:
                    self.lineEdit_calibration_led0_z.setText(str(z))
                elif self.calibration_targets_leg == 1:
                    self.lineEdit_calibration_led1_z.setText(str(z))
                elif self.calibration_targets_leg == 2:
                    self.lineEdit_calibration_led2_z.setText(str(z))
                elif self.calibration_targets_leg == 3:
                    self.lineEdit_calibration_led3_z.setText(str(z))
            #发送指令
            s = self.cmd.ACTION_CALIBRATE + self.cmd.SPLIT + str(self.calibration_targets_leg) + self.cmd.SPLIT + '1' + self.cmd.SPLIT + \
                str(x) + self.cmd.SPLIT + str(y) + self.cmd.SPLIT + str(z) + self.cmd.SPLIT + self.cmd.ENTER
            self.client.send(s.encode('utf-8'))
    # 选择机器狗需要校准的腿，选中则返回编号0-3，没选中则返回-1
    def calibration_target(self, parameter):
        if parameter.objectName() == 'checkBox_calibration_leg0':
            if self.checkBox_calibration_leg0.isChecked() == True:
                self.calibration_targets_leg = 0
            elif self.checkBox_calibration_leg0.isChecked() == False:
                self.calibration_targets_leg = -1
            self.checkBox_calibration_leg1.setChecked(False)
            self.checkBox_calibration_leg2.setChecked(False)
            self.checkBox_calibration_leg3.setChecked(False)
            self.checkBox_calibration_show_1.setChecked(True)
            self.checkBox_calibration_show_2.setChecked(False)
            self.checkBox_calibration_show_3.setChecked(False)
            self.checkBox_calibration_show_4.setChecked(False)
        elif parameter.objectName() == 'checkBox_calibration_leg1':
            if self.checkBox_calibration_leg1.isChecked() == True:
                self.calibration_targets_leg = 1
            elif self.checkBox_calibration_leg1.isChecked() == False:
                self.calibration_targets_leg = -1
            self.checkBox_calibration_leg0.setChecked(False)
            self.checkBox_calibration_leg2.setChecked(False)
            self.checkBox_calibration_leg3.setChecked(False)
            self.checkBox_calibration_show_1.setChecked(False)
            self.checkBox_calibration_show_2.setChecked(True)
            self.checkBox_calibration_show_3.setChecked(False)
            self.checkBox_calibration_show_4.setChecked(False)
        elif parameter.objectName() == 'checkBox_calibration_leg2':
            if self.checkBox_calibration_leg2.isChecked() == True:
                self.calibration_targets_leg = 2
            elif self.checkBox_calibration_leg2.isChecked() == False:
                self.calibration_targets_leg = -1
            self.checkBox_calibration_leg0.setChecked(False)
            self.checkBox_calibration_leg1.setChecked(False)
            self.checkBox_calibration_leg3.setChecked(False)
            self.checkBox_calibration_show_1.setChecked(False)
            self.checkBox_calibration_show_2.setChecked(False)
            self.checkBox_calibration_show_3.setChecked(True)
            self.checkBox_calibration_show_4.setChecked(False)
        elif parameter.objectName() == 'checkBox_calibration_leg3':
            if self.checkBox_calibration_leg3.isChecked() == True:
                self.calibration_targets_leg = 3
            elif self.checkBox_calibration_leg3.isChecked() == False:
                self.calibration_targets_leg = -1
            self.checkBox_calibration_leg0.setChecked(False)
            self.checkBox_calibration_leg1.setChecked(False)
            self.checkBox_calibration_leg2.setChecked(False)
            self.checkBox_calibration_show_1.setChecked(False)
            self.checkBox_calibration_show_2.setChecked(False)
            self.checkBox_calibration_show_3.setChecked(False)
            self.checkBox_calibration_show_4.setChecked(True)

        if self.calibration_targets_leg != -1:
            s = self.cmd.ACTION_CALIBRATE + self.cmd.SPLIT + str(self.calibration_targets_leg) + self.cmd.SPLIT + '3' + self.cmd.SPLIT + \
                str(0) + self.cmd.SPLIT + str(0) + self.cmd.SPLIT + str(0) + self.cmd.SPLIT + self.cmd.ENTER
            self.client.send(s.encode('utf-8'))
            self.timer.start(500)
        elif self.calibration_targets_leg == -1:
            self.timer.stop()
            self.label_picture.setPixmap(QPixmap('./image/esp32_calibration_leg.png'))
            self.checkBox_calibration_show_1.setChecked(False)
            self.checkBox_calibration_show_2.setChecked(False)
            self.checkBox_calibration_show_3.setChecked(False)
            self.checkBox_calibration_show_4.setChecked(False)
    # 使用定时器刷新机器狗图片
    def refresh_image(self):
        if self.calibration_targets_leg == 0:
            if self.calibration_targets_state == 0:
                self.calibration_targets_state = 1
                self.label_picture.setPixmap(QPixmap('./image/esp32_calibration_leg.png'))
            elif self.calibration_targets_state == 1:
                self.calibration_targets_state = 0
                self.label_picture.setPixmap(QPixmap('./image/esp32_calibration_leg0.png'))
        elif self.calibration_targets_leg == 1:
            if self.calibration_targets_state == 0:
                self.calibration_targets_state = 1
                self.label_picture.setPixmap(QPixmap('./image/esp32_calibration_leg.png'))
            elif self.calibration_targets_state == 1:
                self.calibration_targets_state = 0
                self.label_picture.setPixmap(QPixmap('./image/esp32_calibration_leg1.png'))
        elif self.calibration_targets_leg == 2:
            if self.calibration_targets_state == 0:
                self.calibration_targets_state = 1
                self.label_picture.setPixmap(QPixmap('./image/esp32_calibration_leg.png'))
            elif self.calibration_targets_state == 1:
                self.calibration_targets_state = 0
                self.label_picture.setPixmap(QPixmap('./image/esp32_calibration_leg2.png'))
        elif self.calibration_targets_leg == 3:
            if self.calibration_targets_state == 0:
                self.calibration_targets_state = 1
                self.label_picture.setPixmap(QPixmap('./image/esp32_calibration_leg.png'))
            elif self.calibration_targets_state == 1:
                self.calibration_targets_state = 0
                self.label_picture.setPixmap(QPixmap('./image/esp32_calibration_leg3.png'))

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("192.168.1.139",5000))

    calibrationWindow = ESP32_Calibration(s)
    calibrationWindow.setWindowModality(Qt.ApplicationModal)
    calibrationWindow.show()

    #s.close()
    sys.exit(app.exec_())

