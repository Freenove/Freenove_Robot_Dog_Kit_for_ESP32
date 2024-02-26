#!/usr/bin/python3
# -*- coding:utf-8 -*-
# @Time: 2022/05/26
# @Author: 林镇涛
# @Company: Freenove

import sys                                          #添加系统传参对象
import serial                                       #添加系统串口配置对象
import serial.tools.list_ports                      #添加串口工具表对象
from PyQt5 import QtWidgets,QtCore                  #添加窗口对象
from PyQt5.QtGui import QIcon                       #添加图标对象
from ui_configuration import Ui_widget_configuration
from Command import COMMAND                         #导入esptool指令参数
import platform                                     #识别系统平台
from PyQt5.QtWidgets import QMessageBox
from Thread import *
from file import File
import time
import datetime

class ESP32_Configuration(QtWidgets.QWidget, Ui_widget_configuration):
    #定义一个信号通道，用来接受配置界面返回来的ip地址
    configuration_channel = QtCore.pyqtSignal(str)

    def __init__(self):
        super(ESP32_Configuration, self).__init__()
        self.setupUi(self)
        #添加界面的logo
        self.setWindowIcon(QIcon('./image/Freenove.ico'))                     #添加窗口logo

        #变量申请
        self.pyserial=serial.Serial()                                         #串口对象
        self.Com_Dict = {}                                                    #检测所有存在的串口，将信息存储在字典中
        self.cmd = COMMAND()                                                  #命令对象
        self.set_ssid_psd()
        # 设置信号与槽函数
        self.connect()

        #初始化配置部分
        #self.port_check()                                                     #扫描串口并加载到串口列表
        #self.print_information()                                              #打印信息提示框

    def set_ssid_psd(self):
        self.file = File()
        data = self.file.read_txt()
        if data != -1:
            ssid = data[0].split('#')
            psd = data[1].split('#')
            self.lineEdit_configuration_wifi_ssid.setText(ssid[1])
            self.lineEdit_configuration_wifi_password.setText(psd[1])
    #打印提示信息
    def print_information(self):
        print('The operating system is: ' + platform.system())
        QMessageBox.information(self, "Freenove", "Please connect ESP32 to your computer.")

    #信号与槽部分
    def connect(self):
        self.button_configuration_scan_serial.clicked.connect(self.port_check)                   #检测串口并重新更新到串口下拉框中
        self.button_configuration_config_wifi.clicked.connect(self.port_config)                  #通过串口将wifi配置信息发送给机器狗

    #串口检测
    def port_check(self):
        port_list = list(serial.tools.list_ports.comports())        #获取串口端口号并强制转化为列表并存入port_list
        self.combobox_configuration_serial_port.clear()                 #清除串口端口列表数据
        for port in port_list:                                          #解析串口列表数据，将其存入字典Com_Dict，并添加到屏幕端口号的下拉框中
            if port[0] != 'COM1':
                self.Com_Dict["%s" % port[0]] = "%s" % port[1]              #将串口列表数据进行处理
                self.combobox_configuration_serial_port.addItem(port[0])    #将串口数据添加到控件中
        if len(self.Com_Dict) == 0:                                     #如果没有检测到端口，使用消息框打印提示信息，检测不到串口
            QMessageBox.information(self, "Freenove", "Cannot detect serial port!\r\nPlease connect ESP32 to your computer.")
            return None
        else:
            self.pyserial.baudrate = 115200
            self.pyserial.bytesize = 8
            self.pyserial.stopbits = 1
            self.pyserial.parity = "N"
            self.current_port_flag = 0
            for port in port_list:
                self.pyserial.port = port[0]
                try:
                    self.pyserial.open()
                    self.pyserial.write(("W#2#\r\n").encode('utf-8'))
                    time.sleep(0.1)
                    num = self.pyserial.inWaiting()
                    if num != 0:
                        command = self.pyserial.read(num)
                        str_command = command.decode('utf-8')
                        if str_command.find('FREENOVE-DOG') != -1:
                            self.current_port = port[0]
                            self.current_port_flag = 1
                    self.pyserial.close()
                except:
                    pass
            if self.current_port_flag == 1:
                self.combobox_configuration_serial_port.setCurrentText(self.current_port)
                QMessageBox.information(self, "Freenove", "Find the robot port: "+self.current_port)
            else:
                QMessageBox.information(self, "Freenove", "No robot port founds!")
                pass

    #将wifi信息发送给机器狗
    def port_config(self):
        self.pyserial.port = self.combobox_configuration_serial_port.currentText()
        self.pyserial.baudrate = 115200
        self.pyserial.bytesize = 8
        self.pyserial.stopbits = 1
        self.pyserial.parity = "N"
        #如果打不开串口，提示串口打开失败
        try:
            self.pyserial.open()
        except:
            QMessageBox.information(self,"Freenove","Open Serial Error!")
            return None
        #打开串口成功，清空发送和接收缓冲区
        self.pyserial.reset_output_buffer()
        self.pyserial.reset_input_buffer()
        #如果打开的串口并不是连接机器狗的串口，打印提示信息并退出
        if self.pyserial.port != self.current_port:
            QMessageBox.information(self, "Freenove", "Please check whether this port is connected to the robot dog!")
            self.pyserial.close()
            return None
        #如果打开的串口连接着机器狗
        else:
            #判断串口是否已经打开
            if self.pyserial.isOpen():
                self.lineEdit_configuration_ip_address.setText('')
                #发送指令关闭机器狗的STA和AP模式
                #command = self.cmd.ACTION_NETWORK + self.cmd.SPLIT + "2" + self.cmd.SPLIT + self.cmd.ENTER
                #self.pyserial.write(command.encode('utf-8'))
                #command = self.cmd.ACTION_NETWORK + self.cmd.SPLIT + '4' + self.cmd.SPLIT + self.cmd.ENTER
                #self.pyserial.write(command.encode('utf-8'))
                #如果选择STA模式
                if self.combobox_configuration_wifi_mode.currentText() == "STA Mode":
                    self.lineEdit_configuration_wifi_state.setText('Configure the WiFi')
                    QMessageBox.information(self, "Freenove", "It will take a few minutes to configure wifi.\r\nPlease be patient.")
                    #获取界面的wifi名称和密码，通过串口发送配置指令
                    ssid = self.lineEdit_configuration_wifi_ssid.text()
                    password = self.lineEdit_configuration_wifi_password.text()
                    command = self.cmd.ACTION_NETWORK + self.cmd.SPLIT + "1" + self.cmd.SPLIT + ssid + self.cmd.SPLIT + password + self.cmd.SPLIT + self.cmd.ENTER #N#1#ssid#password#\r\n
                    self.pyserial.write(command.encode('utf-8'))
                    #接收来自机器狗的反馈信息
                    self.start_time = int(time.time())
                    num = 0
                    while True:
                        command = self.pyserial.readline()
                        str_command = command.decode('utf-8')
                        #print(str_command)
                        #如果接收到IP地址指令，将其显示到界面上，并退出循环
                        if str_command.find('N#101#') != -1:
                            str_list_cmd = str_command.split('#')
                            #print(str_list_cmd)
                            self.lineEdit_configuration_ip_address.setText(str_list_cmd[3])
                            self.lineEdit_configuration_wifi_state.setText('Success')
                            ssid = self.lineEdit_configuration_wifi_ssid.text()
                            psd = self.lineEdit_configuration_wifi_password.text()
                            ip = self.lineEdit_configuration_ip_address.text()
                            self.file.write_txt(ssid,psd,ip)
                            QMessageBox.information(self, "Freenove", "Robot wifi is configured successfully!")
                            break
                        #如果wifi配置失败，提示用户检查名称和密码
                        elif str_command.find('WiFi connect failed!') != -1:
                            self.lineEdit_configuration_wifi_state.setText('Failed')
                            QMessageBox.information(self, "Freenove", "Wifi configuration failed, please check ssid and password!")
                            break
                        #如果是其他反馈指令，不理会，继续读取下一行
                        else:
                            self.current_time = int(time.time())
                            if (self.current_time - self.start_time) > 20:
                                print("Configuration failure: Timeout.")
                                self.lineEdit_configuration_wifi_state.setText('Failed')
                                QMessageBox.information(self, "Freenove", "Wifi configuration failed, please check ssid and password!")
                                break
                            else:
                                continue
                #如果选择AP模式
                elif self.combobox_configuration_wifi_mode.currentText() == "AP Mode":
                    #发送开启WiFi-AP模式指令
                    command = self.cmd.ACTION_NETWORK + self.cmd.SPLIT + "3" + self.cmd.SPLIT + self.cmd.ENTER
                    self.pyserial.write(command.encode('utf-8'))
                    #接收来自机器狗的反馈信息
                    while True:
                        command = self.pyserial.readline()
                        str_command = command.decode('utf-8')
                        #print(str_command)
                        #如果接收到wifi-AP模式开启成功信息，打印配置成功提示信息
                        if str_command.find('N#301#') != -1:
                            str_list_cmd = str_command.split("#")
                            #print(str_list_cmd)
                            #根据反馈信息，将机器狗的wifi名称，密码，ip地址显示在界面上
                            self.lineEdit_configuration_wifi_ssid.setText(str_list_cmd[2])
                            self.lineEdit_configuration_ip_address.setText(str_list_cmd[3])
                            self.lineEdit_configuration_wifi_password.setText(str_list_cmd[4])
                            self.lineEdit_configuration_wifi_state.setText('Success')
                            QMessageBox.information(self, "Freenove", "Robot wifi is configured successfully")
                            break
                        #如果接收到AP模式开启失败信息，打印配置失败提示信息
                        elif str_command.find('N#302#') != -1:
                            self.lineEdit_configuration_wifi_state.setText('Failed')
                            QMessageBox.information(self, "Freenove", "WiFi AP mode failed to be enabled!\r\nPlease try again!")
                            break
                        else:
                            continue
                self.pyserial.close()
            else:
                QMessageBox.information(self, "Freenove", "Please open the serial port first!")
                return None

    def closeEvent(self, event):
        if self.lineEdit_configuration_ip_address.text()!="":
            self.configuration_channel.emit(self.lineEdit_configuration_ip_address.text())
        event.accept()

def print_ip_address(ip):
    print(ip)

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    myshow = ESP32_Configuration()
    myshow.show()
    myshow.configuration_channel.connect(print_ip_address)
    sys.exit(app.exec_())
