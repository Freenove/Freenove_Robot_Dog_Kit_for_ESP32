import sys
from threading import Thread
import socket
import struct
import cv2
import numpy as np
import io
from PIL import Image
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5 import QtWidgets,QtCore

from Thread import *
from Command import COMMAND
from file import File
from ui_main import Ui_form_main
from calibration import ESP32_Calibration
from firmware import ESP32_Firmware
from configuration import ESP32_Configuration
from led import ESP32_LED
from action import ESP32_ACTION

class ESP32_MainWindow(QtWidgets.QMainWindow,Ui_form_main):
    power_chn = QtCore.pyqtSignal(int)
    def __init__(self):
        super(ESP32_MainWindow,self).__init__()
        self.setupUi(self)
        self.setWindowIcon(QIcon('./image/Freenove.ico'))#添加窗口logo
        self.label_main_video.setPixmap(QPixmap('./image/esp32_dog_main.png'))
        ipValidator = QRegExpValidator(QRegExp('^((2[0-4]\d|25[0-5]|\d?\d|1\d{2})\.){3}(2[0-4]\d|25[0-5]|[01]?\d\d?)$'))
        self.lineEdit_main_ip_address.setValidator(ipValidator)
        self.camera_angle = 0
        self.robot_mode = 1
        self.button_main_move_mode.setEnabled(False)
        self.cmd = COMMAND()
        self.set_ip_address()
        # 设置信号与槽函数
        self.connect()
        self.grabKeyboard()#多界面时需要调用这句，否则只能监听到一次按下事件
        self.window_num = 0

    #控件关联到槽函数
    def connect(self):
        self.button_main_firmware.clicked.connect(self.show_firmware)        #关联firmware界面
        self.button_main_configure.clicked.connect(self.show_configuration)  #关联configuration界面
        self.button_main_calibration.clicked.connect(self.show_calibration)  #关联到校准界面
        self.button_main_led.clicked.connect(self.show_led)                  #关联到彩灯界面
        self.button_main_action.clicked.connect(self.show_action)            #关联到互动界面
        self.button_main_connect.clicked.connect(self.wifi_connect)          #关联到WiFi连接函数
        self.button_main_video.clicked.connect(self.wifi_video)              #关联到切换video控件显示字符函数
        self.button_main_original_degree.clicked.connect(lambda: self.wifi_rotate_video(self.button_main_original_degree)) # 回到原始位置
        self.button_main_ccw_90_degree.clicked.connect(lambda: self.wifi_rotate_video(self.button_main_ccw_90_degree))     # 逆时针旋转90度
        self.button_main_cw_90_degree.clicked.connect(lambda: self.wifi_rotate_video(self.button_main_cw_90_degree))       # 顺时针旋转90度
        self.button_main_buzzer.pressed.connect(self.send_buzzer_cmd)        #蜂鸣器按下发送指令
        self.button_main_buzzer.released.connect(self.send_buzzer_cmd)       #蜂鸣器松开发送指令
        self.button_main_sonar_mode.clicked.connect(self.send_sonar_mode_cmd)#发送超声波避障模式指令
        self.button_main_relax.clicked.connect(self.send_relax_robot_cmd)    #发送舵机卸力指令
        self.horizontalSlider_main_step.valueChanged.connect(lambda: self.slider_set_data(self.horizontalSlider_main_step))  #步长滑条控件显示
        self.horizontalSlider_main_gama.valueChanged.connect(lambda: self.slider_set_data(self.horizontalSlider_main_gama))  #自转范围滑条显示
        self.horizontalSlider_main_speed.valueChanged.connect(lambda: self.slider_set_data(self.horizontalSlider_main_speed))#速度滑条控件显示

        self.button_main_move_mode.clicked.connect(lambda: self.robot_run_mode(self.button_main_move_mode))                  #机器狗运动模式选择
        self.button_main_twist_mode.clicked.connect(lambda: self.robot_run_mode(self.button_main_twist_mode))                #机器狗姿态扭动模式选择

        self.button_main_forward.pressed.connect(lambda: self.send_robot_run_cmd(self.button_main_forward))                  #发送机器狗运动指令
        self.button_main_forward.released.connect(lambda: self.send_robot_run_cmd(self.button_main_forward))
        self.button_main_backward.pressed.connect(lambda: self.send_robot_run_cmd(self.button_main_backward))
        self.button_main_backward.released.connect(lambda: self.send_robot_run_cmd(self.button_main_backward))
        self.button_main_left_shift.pressed.connect(lambda: self.send_robot_run_cmd(self.button_main_left_shift))
        self.button_main_left_shift.released.connect(lambda: self.send_robot_run_cmd(self.button_main_left_shift))
        self.button_main_right_shift.pressed.connect(lambda: self.send_robot_run_cmd(self.button_main_right_shift))
        self.button_main_right_shift.released.connect(lambda: self.send_robot_run_cmd(self.button_main_right_shift))
        self.button_main_turn_left.pressed.connect(lambda: self.send_robot_run_cmd(self.button_main_turn_left))
        self.button_main_turn_left.released.connect(lambda: self.send_robot_run_cmd(self.button_main_turn_left))
        self.button_main_turn_right.pressed.connect(lambda: self.send_robot_run_cmd(self.button_main_turn_right))
        self.button_main_turn_right.released.connect(lambda: self.send_robot_run_cmd(self.button_main_turn_right))

        self.power_chn.connect(self.show_battery_percentage)                                                                #设置电池电量百分比显示

    #显示电池电量
    def show_battery_percentage(self, value):
        try:
            self.progress_Power_main_percentage.setValue(value)
        except:
            pass
    #键盘按下事件
    def keyPressEvent(self, event):
        if event.isAutoRepeat():#长按事件，机器狗不需要重复发指令，长按不需要发送命令
            if (event.key() == Qt.Key.Key_Up):
                i = self.horizontalSlider_main_step.value()
                if i < 40:
                    i = i + 1
                    self.horizontalSlider_main_step.setValue(i)
            elif (event.key() == Qt.Key.Key_Down):
                i = self.horizontalSlider_main_step.value()
                if i > 1:
                    i = i - 1
                    self.horizontalSlider_main_step.setValue(i)
            elif (event.key() == Qt.Key.Key_Left):
                i = self.horizontalSlider_main_gama.value()
                if i > 0:
                    i = i - 1
                    self.horizontalSlider_main_gama.setValue(i)
            elif (event.key() == Qt.Key.Key_Right):
                i = self.horizontalSlider_main_gama.value()
                if i < 20:
                    i = i + 1
                    self.horizontalSlider_main_gama.setValue(i)
            elif (event.key() == Qt.Key.Key_PageUp):
                i = self.horizontalSlider_main_speed.value()
                if i < 8:
                    i = i + 1
                    self.horizontalSlider_main_speed.setValue(i)
            elif (event.key() == Qt.Key.Key_PageDown):
                i = self.horizontalSlider_main_speed.value()
                if i > 1:
                    i = i - 1
                    self.horizontalSlider_main_speed.setValue(i)
        else:
            flag=0
            alpha = '0'
            stepLength = self.lineEdit_main_step.text()
            gama = '0'
            spd = self.lineEdit_main_speed.text()
            pitch = '0'
            roll = '0'
            yaw = '0'
            #机器狗按键控制部分
            if (event.key() == Qt.Key.Key_W):
                flag = 1
                alpha = '0'  # 固定角度为向前
                pitch = str(self.horizontalSlider_main_gama.value() * (-2))  # 姿态扭动幅度可由滑条进行调节
                self.button_main_forward.setDown(True)
            elif (event.key() == Qt.Key.Key_S):
                flag = 1
                alpha = '180'  # 固定角度为向后
                pitch = str(self.horizontalSlider_main_gama.value() * (2))  # 姿态扭动幅度可由滑条进行调节
                self.button_main_backward.setDown(True)
            elif (event.key() == Qt.Key.Key_A):
                flag = 1
                alpha = '90'  # 固定角度为向左平移
                roll = str(self.horizontalSlider_main_gama.value() * (-1))  # 姿态扭动幅度可由滑条进行调节
                self.button_main_left_shift.setDown(True)
            elif (event.key() == Qt.Key.Key_D):
                flag = 1
                alpha = '-90'  # 固定角度为向右平移
                roll = str(self.horizontalSlider_main_gama.value())  # 姿态扭动幅度可由滑条进行调节
                self.button_main_right_shift.setDown(True)
            elif (event.key() == Qt.Key.Key_Q):
                flag = 1
                gama = str(self.horizontalSlider_main_gama.value())  # 转动角度可由滑条进行调节
                yaw = str(self.horizontalSlider_main_gama.value())  # 姿态扭动幅度可由滑条进行调节
                self.button_main_turn_left.setDown(True)
            elif (event.key() == Qt.Key.Key_E):
                flag = 1
                gama = str(self.horizontalSlider_main_gama.value() * (-1))  # 转动角度可由滑条进行调节
                yaw = str(self.horizontalSlider_main_gama.value() * (-1))  # 姿态扭动幅度可由滑条进行调节
                self.button_main_turn_right.setDown(True)

            #子窗口配置部分
            elif (event.key() == Qt.Key.Key_7):
                self.window_num = 1
                self.show_firmware()
            elif (event.key() == Qt.Key.Key_8):
                self.window_num = 2
                self.show_configuration()
            elif (event.key() == Qt.Key.Key_9):
                self.window_num = 3
                self.show_led()
            elif (event.key() == Qt.Key.Key_5):
                self.window_num = 4
                self.show_calibration()
            elif (event.key() == Qt.Key.Key_6):
                self.window_num = 5
                self.show_action()

            #蜂鸣器按键
            elif (event.key() == Qt.Key.Key_4):
                self.button_main_buzzer.setDown(True)
                self.send_buzzer_cmd()
            #机器狗模式选择部分
            elif (event.key() == Qt.Key.Key_1):
                self.robot_mode = 1
                self.button_main_move_mode.setEnabled(False)
                self.button_main_twist_mode.setEnabled(True)
                self.button_main_sonar_mode.setEnabled(True)
                self.robot_text_switch(self.robot_mode)
            elif (event.key() == Qt.Key.Key_2):
                self.robot_mode = 2
                self.button_main_move_mode.setEnabled(True)
                self.button_main_twist_mode.setEnabled(False)
                self.button_main_sonar_mode.setEnabled(True)
                self.robot_text_switch(self.robot_mode)
            elif (event.key() == Qt.Key.Key_3):
                self.send_sonar_mode_cmd()
                pass

            #舵机扭力控制
            elif (event.key() == Qt.Key.Key_Space):
                self.button_main_relax.setDown(True)
                self.send_relax_robot_cmd()

            #按键控制滑条部分
            elif (event.key() == Qt.Key.Key_Up):
                i = self.horizontalSlider_main_step.value()
                if i < 40:
                    i = i+1
                    self.horizontalSlider_main_step.setValue(i)
            elif (event.key() == Qt.Key.Key_Down):
                i = self.horizontalSlider_main_step.value()
                if i > 1:
                    i = i-1
                    self.horizontalSlider_main_step.setValue(i)
            elif (event.key() == Qt.Key.Key_Left):
                i = self.horizontalSlider_main_gama.value()
                if i > 0:
                    i = i-1
                    self.horizontalSlider_main_gama.setValue(i)
            elif (event.key() == Qt.Key.Key_Right):
                i = self.horizontalSlider_main_gama.value()
                if i < 20:
                    i = i+1
                    self.horizontalSlider_main_gama.setValue(i)
            elif (event.key() == Qt.Key.Key_PageUp):
                i = self.horizontalSlider_main_speed.value()
                if i < 8:
                    i = i + 1
                    self.horizontalSlider_main_speed.setValue(i)
            elif (event.key() == Qt.Key.Key_PageDown):
                i = self.horizontalSlider_main_speed.value()
                if i > 1:
                    i = i - 1
                    self.horizontalSlider_main_speed.setValue(i)

            #界面关闭快捷键
            elif (event.key() == Qt.Key.Key_Escape):
                if self.window_num == 1:
                    try:
                        self.firmwareWindow.close()
                    except:
                        pass
                elif self.window_num == 2:
                    try:
                        self.configurationWindow.close()
                    except:
                        pass
                elif self.window_num == 3:
                    try:
                        self.ledWindow.close()
                    except:
                        pass
                elif self.window_num == 4:
                    try:
                        self.calibrationWindow.close()
                    except:
                        pass
                elif self.window_num == 5:
                    try:
                        self.actionWindow.close()
                    except:
                        pass

            if flag == 1:
                if self.robot_mode == 1:
                    s = self.cmd.ACTION_MOVE_ANY + self.cmd.SPLIT + alpha + self.cmd.SPLIT + stepLength + self.cmd.SPLIT + gama + self.cmd.SPLIT + spd + self.cmd.SPLIT + self.cmd.ENTER
                elif self.robot_mode == 2:
                    s = self.cmd.ACTION_TWIST + self.cmd.SPLIT + pitch + self.cmd.SPLIT + roll + self.cmd.SPLIT + yaw + self.cmd.SPLIT + self.cmd.ENTER
                try:
                    self.socket_command_send(s)
                except:
                    pass
    #键盘松开事件
    def keyReleaseEvent(self, event):
        if event.isAutoRepeat():
            pass
        else:
            flag=0
            alpha = '0'
            stepLength = self.lineEdit_main_step.text()
            gama = '0'
            spd = self.lineEdit_main_speed.text()
            pitch = '0'
            roll = '0'
            yaw = '0'
            if (event.key() == Qt.Key.Key_W):
                flag = 1
                alpha = '0'  # 固定角度为向前
                pitch = str(self.horizontalSlider_main_gama.value() * (-2))  # 姿态扭动幅度可由滑条进行调节
                self.button_main_forward.setDown(False)
            elif (event.key() == Qt.Key.Key_S):
                flag = 1
                alpha = '180'  # 固定角度为向后
                pitch = str(self.horizontalSlider_main_gama.value() * (2))  # 姿态扭动幅度可由滑条进行调节
                self.button_main_backward.setDown(False)
            elif (event.key() == Qt.Key.Key_A):
                flag = 1
                alpha = '90'  # 固定角度为向左平移
                roll = str(self.horizontalSlider_main_gama.value() * (-1))  # 姿态扭动幅度可由滑条进行调节
                self.button_main_left_shift.setDown(False)
            elif (event.key() == Qt.Key.Key_D):
                flag = 1
                alpha = '-90'  # 固定角度为向右平移
                roll = str(self.horizontalSlider_main_gama.value())  # 姿态扭动幅度可由滑条进行调节
                self.button_main_right_shift.setDown(False)
            elif (event.key() == Qt.Key.Key_Q):
                flag = 1
                gama = str(self.horizontalSlider_main_gama.value())  # 转动角度可由滑条进行调节
                yaw = str(self.horizontalSlider_main_gama.value())  # 姿态扭动幅度可由滑条进行调节
                self.button_main_turn_left.setDown(False)
            elif (event.key() == Qt.Key.Key_E):
                flag = 1
                gama = str(self.horizontalSlider_main_gama.value() * (-1))  # 转动角度可由滑条进行调节
                yaw = str(self.horizontalSlider_main_gama.value() * (-1))  # 姿态扭动幅度可由滑条进行调节
                self.button_main_turn_right.setDown(False)
            elif (event.key() == Qt.Key.Key_4):
                self.button_main_buzzer.setDown(False)
                self.send_buzzer_cmd()
            elif (event.key() == Qt.Key.Key_Space):
                self.button_main_relax.setDown(False)

            if flag == 1:
                if self.robot_mode == 1:
                    s = self.cmd.ACTION_MOVE_ANY + self.cmd.SPLIT + '0' + self.cmd.SPLIT + '0' + self.cmd.SPLIT + '0' + self.cmd.SPLIT + spd + self.cmd.SPLIT + self.cmd.ENTER
                elif self.robot_mode == 2:
                    s = self.cmd.ACTION_TWIST + self.cmd.SPLIT + '0' + self.cmd.SPLIT + '0' + self.cmd.SPLIT + '0' + self.cmd.SPLIT + self.cmd.ENTER
                try:
                    self.socket_command_send(s)
                except:
                    pass
    #设置WiFi的IP地址
    def set_ip_address(self):
        self.file = File()
        data = self.file.read_txt()
        if data != -1:
            ip = data[2].split('#')
            #print(ip[1])
            self.lineEdit_main_ip_address.setText(ip[1])

    #按键调用firmware界面函数
    def show_firmware(self):
        try:
            self.window_num = 1
            self.firmwareWindow = ESP32_Firmware()
            self.firmwareWindow.setWindowModality(Qt.ApplicationModal)
            self.firmwareWindow.show()
        except Exception as e:
            print(e)
    #按键调用configuration界面函数
    def show_configuration(self):
        try:
            self.window_num = 2
            self.releaseKeyboard()
            self.configurationWindow = ESP32_Configuration()
            #定义个函数：子窗口信号通道连接到这个函数上，该函数接收来自窗口的IP地址。
            self.configurationWindow.setWindowModality(Qt.ApplicationModal)
            self.configurationWindow.show()
            self.configurationWindow.configuration_channel.connect(self.set_main_ip_address)
        except Exception as e:
            print(e)
    #获取WiFi配置窗口返回值，配置主界面
    def set_main_ip_address(self,ip):
        try:
            self.lineEdit_main_ip_address.setText(ip)
            self.grabKeyboard()
            self.window_num = 0
        except:
            pass
    #校准界面
    def show_calibration(self):
        if self.button_main_connect.text() == 'Disconnect':
            self.window_num = 4
            self.calibrationWindow=ESP32_Calibration(self.client_socket_command)
            self.calibrationWindow.setWindowModality(Qt.ApplicationModal)
            self.calibrationWindow.show()
        else:
            QMessageBox.information(self, "Freenove", "Please connect WiFi first.")
    #彩灯界面
    def show_led(self):
        if self.button_main_connect.text() == 'Disconnect':
            self.window_num = 3
            self.ledWindow= ESP32_LED(self.client_socket_command)
            self.ledWindow.setWindowModality(Qt.ApplicationModal)
            self.ledWindow.show()
        else:
            QMessageBox.information(self, "Freenove", "Please connect WiFi first.")
    #互动界面
    def show_action(self):
        if self.button_main_connect.text() == 'Disconnect':
            self.window_num = 5
            self.actionWindow = ESP32_ACTION(self.client_socket_command)
            self.actionWindow.setWindowModality(Qt.ApplicationModal)
            self.actionWindow.show()
        else:
            QMessageBox.information(self, "Freenove", "Please connect WiFi first.")

    #打开socket函数
    def socket_start(self):
        self.client_socket_video = socket.socket(socket.AF_INET, socket.SOCK_STREAM)    #视频传输
        self.client_socket_command = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  #指令传输
    #关闭socke函数
    def socket_close(self):
        try:
            self.client_socket_video.shutdown(2)
            self.client_socket_command.shutdown(2)
            self.client_socket_video.close()
            self.client_socket_command.close()
        except:
            pass
    #检查视频图像完整性
    def socket_image_4_butes(self, buf):
        bValid = True
        if buf[6:10] in (b'JFIF', b'Exif'):
            if not buf.rstrip(b'\0\r\n').endswith(b'\xff\xd9'):
                bValid = False
        else:
            try:
                Image.open(io.BytesIO(buf)).verify()
            except:
                bValid = False
        return bValid
    #视频传输
    def socket_streaming(self, ip):
        try:
            self.client_socket_video.connect((ip, 8000))
            self.connection = self.client_socket_video.makefile('rb')
        except Exception as e:
            #print(e)
            # print("Failed to create the video transmission thread. It is recommended to reset the dog and try again.")
            self.button_main_video.setText('Open video')
            return None
        while True:
            try:
                stream_bytes = self.connection.read(4)
                leng = struct.unpack('<L', stream_bytes[:4])
                jpg = self.connection.read(leng[0])
                if self.socket_image_4_butes(jpg):
                    buf_image = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
                    height, width, bytesPerComponent = buf_image.shape
                    cv2.cvtColor(buf_image, cv2.COLOR_BGR2RGB, buf_image)
                    QImg = QImage(buf_image.data, width, height, 3 * width, QImage.Format_RGB888)
                    transform = QTransform()
                    transform.scale(1, 1)
                    transform.rotate(self.camera_angle)
                    QImg = QImg.transformed(transform)
                    if self.button_main_video.text() == 'Close video':
                        self.label_main_video.setPixmap(QPixmap.fromImage(QImg))
            except Exception as e:
                print(e)
                break
    #指令接收
    def socket_recv_data(self, ip):
        try:
            self.client_socket_command.connect((ip, 5000))
            print ("Connection Successful!")
        except Exception as e:
            #print(e)
            print ("Connect to server Faild!: Server IP is right? Server is opend?")
            self.button_main_connect.setText('Connect')
            #QMessageBox.information(self, "Freenove", "WiFi connection failed. Please check whether the IP address is correct.")

            return None
        while True:
            try:
                self.data=self.client_socket_command.recv(1024).decode('utf-8')
                if self.data.find('I#')!=-1:
                    s = self.data.split('#')
                    try:
                        #self.progress_Power_main_percentage.setValue(int(s[2]))
                        self.power_chn.emit(int(s[2]))
                    except:
                        pass
                elif self.data.find('H#')!=-1:
                    s = self.data.split('#')
                    try:
                        buf = s[1] + 'cm'
                        self.lineEdit_main_sonar_distance.setText(buf)
                    except:
                        pass
                pass
            except Exception as e:
                print(e)
    #指令发送
    def socket_command_send(self,s):
        self.client_socket_command.send(s.encode('utf-8'))

    #WiFi连接函数
    def wifi_connect(self):
        if self.button_main_connect.text() == 'Connect':
            self.ip = self.lineEdit_main_ip_address.text()
            if self.ip == 'IP Address':
                QMessageBox.information(self, "Freenove", "Please enter the IP address first.")
            else:
                self.socket_start()
                try:
                    self.recv_data = Thread(target=self.socket_recv_data, args=(self.ip,))
                    self.recv_data.start()
                except:
                    print('recv error')
                try:
                    self.streaming = Thread(target=self.socket_streaming,args=(self.ip,))
                    self.streaming.start()
                except:
                    print('video error')
                self.button_main_connect.setText('Disconnect')
                print('Server address:' + str(self.ip) + '\n')

        elif self.button_main_connect.text() == 'Disconnect':
            self.button_main_connect.setText('Connect')
            self.label_main_video.setPixmap(QPixmap('./image/esp32_dog_main.png'))
            try:
                stop_thread(self.streaming)
                stop_thread(self.recv_data)
            except:
                pass
            self.socket_close()
            print('The WiFi connection is disconnected.')
    #切换video控件显示字符函数
    def wifi_video(self):
        if self.button_main_video.text() == 'Open video':
            if self.button_main_connect.text() == 'Disconnect':
                self.button_main_video.setText('Close video')
            else:
                QMessageBox.information(self,"Freenove","Make sure wifi is connected!")
        elif self.button_main_video.text() == 'Close video':
            self.button_main_video.setText('Open video')
            self.label_main_video.setPixmap(QPixmap('./image/esp32_dog_main.png'))
    #旋转视频显示界面
    def wifi_rotate_video(self, parameter):
        if parameter.text() == 'Original':
            self.camera_angle = 0
        elif parameter.text() == 'CCW-90°':
            self.camera_angle -= 90
            if self.camera_angle <= -360:
                self.camera_angle = 0
        elif parameter.text() == 'CW-90°':
            self.camera_angle += 90
            if self.camera_angle >= 360:
                self.camera_angle = 0

    #发送蜂鸣器指令
    def send_buzzer_cmd(self):
        if self.button_main_connect.text() == 'Disconnect':
            s = ''
            if self.button_main_buzzer.isDown() == True:
                s = self.cmd.ACTION_BUZZER + self.cmd.SPLIT + '2500' + self.cmd.SPLIT + self.cmd.ENTER
            elif self.button_main_buzzer.isDown() == False:
                s = self.cmd.ACTION_BUZZER + self.cmd.SPLIT + '0' + self.cmd.SPLIT + self.cmd.ENTER
            try:
                self.socket_command_send(s)
            except:
                pass
    #发送超声波避障模式指令
    def send_sonar_mode_cmd(self):
        self.robot_mode = 3
        self.button_main_move_mode.setEnabled(True)
        self.button_main_twist_mode.setEnabled(True)
        self.button_main_sonar_mode.setEnabled(False)
        s = self.cmd.ACTION_AUTO_WALKING + self.cmd.SPLIT + '1' + self.cmd.SPLIT + self.cmd.ENTER
        try:
            self.socket_command_send(s)
        except:
            pass
    #发送舵机卸力指令
    def send_relax_robot_cmd(self):
        s = self.cmd.ACTION_UP_DOWN + self.cmd.SPLIT + '0' + self.cmd.SPLIT + self.cmd.ENTER
        try:
            self.socket_command_send(s)
        except:
            pass
        pass
    #滑条设置
    def slider_set_data(self, parameter):
        if parameter.objectName() == 'horizontalSlider_main_speed':
            self.lineEdit_main_speed.setText(str(self.horizontalSlider_main_speed.value()))
        elif parameter.objectName() == 'horizontalSlider_main_step':
            self.lineEdit_main_step.setText(str(self.horizontalSlider_main_step.value()))
        elif parameter.objectName() == 'horizontalSlider_main_gama':
            self.lineEdit_main_gama.setText(str(self.horizontalSlider_main_gama.value()))
    #机器狗运动模式选择
    def robot_run_mode(self, parameter):
        if parameter.objectName() == 'button_main_move_mode':
            self.robot_mode = 1
            self.button_main_move_mode.setEnabled(False)
            self.button_main_twist_mode.setEnabled(True)
            self.button_main_sonar_mode.setEnabled(True)
        elif parameter.objectName() == 'button_main_twist_mode':
            self.robot_mode = 2
            self.button_main_move_mode.setEnabled(True)
            self.button_main_twist_mode.setEnabled(False)
            self.button_main_sonar_mode.setEnabled(True)
        self.robot_text_switch(self.robot_mode)
    #机器狗运动模式与姿态模式控件文本显示
    def robot_text_switch(self, parameter):
        if parameter == 1:
            self.button_main_forward.setText('ForWard')
            self.button_main_turn_left.setText('Turn Left')
            self.button_main_left_shift.setText('Left Shift')
            self.button_main_turn_right.setText('Turn Right')
            self.button_main_right_shift.setText('Right Shift')
            self.button_main_backward.setText('BackWard')
            self.label_main_gama.setText('Gama')
        elif parameter == 2:
            self.button_main_forward.setText('Front')
            self.button_main_turn_left.setText('CCW')
            self.button_main_left_shift.setText('Left')
            self.button_main_turn_right.setText('CW')
            self.button_main_right_shift.setText('Right')
            self.button_main_backward.setText('Back')
            self.label_main_gama.setText('Twist')
        try:
            s = self.cmd.ACTION_AUTO_WALKING + self.cmd.SPLIT + '0' + self.cmd.SPLIT + self.cmd.ENTER
            self.socket_command_send(s)
        except:
            pass
    #机器狗移动指令
    def send_robot_run_cmd(self, parameter):
        alpha = '0'
        stepLength = self.lineEdit_main_step.text()
        gama = '0'
        spd = self.lineEdit_main_speed.text()
        pitch = '0'
        roll = '0'
        yaw = '0'
        if parameter.objectName() == 'button_main_forward':
            alpha = '0'    #固定角度为向前
            pitch = str(self.horizontalSlider_main_gama.value()*(-2)) #姿态扭动幅度可由滑条进行调节
        elif parameter.objectName() == 'button_main_backward':
            alpha = '180'  #固定角度为向后
            pitch = str(self.horizontalSlider_main_gama.value()*(2))  #姿态扭动幅度可由滑条进行调节
        elif parameter.objectName() == 'button_main_left_shift':
            alpha = '90'   #固定角度为向左平移
            roll = str(self.horizontalSlider_main_gama.value()*(-1)) #姿态扭动幅度可由滑条进行调节
        elif parameter.objectName() == 'button_main_right_shift':
            alpha = '-90'  #固定角度为向右平移
            roll = str(self.horizontalSlider_main_gama.value())     #姿态扭动幅度可由滑条进行调节
        elif parameter.objectName() == 'button_main_turn_left':
            gama = str(self.horizontalSlider_main_gama.value())     #转动角度可由滑条进行调节
            yaw = str(self.horizontalSlider_main_gama.value())      #姿态扭动幅度可由滑条进行调节
        elif parameter.objectName() == 'button_main_turn_right':
            gama = str(self.horizontalSlider_main_gama.value()*(-1))#转动角度可由滑条进行调节
            yaw = str(self.horizontalSlider_main_gama.value()*(-1)) #姿态扭动幅度可由滑条进行调节
        if self.robot_mode == 1:
            if parameter.isDown() == True:
                s = self.cmd.ACTION_MOVE_ANY + self.cmd.SPLIT + alpha + self.cmd.SPLIT + stepLength + self.cmd.SPLIT + gama + self.cmd.SPLIT + spd + self.cmd.SPLIT + self.cmd.ENTER
            elif parameter.isDown() == False:
                s = self.cmd.ACTION_MOVE_ANY + self.cmd.SPLIT + '0' + self.cmd.SPLIT + '0' + self.cmd.SPLIT + '0' + self.cmd.SPLIT + spd + self.cmd.SPLIT + self.cmd.ENTER
        elif self.robot_mode == 2:
            if parameter.isDown() == True:
                s = self.cmd.ACTION_TWIST + self.cmd.SPLIT + pitch + self.cmd.SPLIT + roll + self.cmd.SPLIT + yaw + self.cmd.SPLIT + self.cmd.ENTER
            elif parameter.isDown() == False:
                s = self.cmd.ACTION_TWIST + self.cmd.SPLIT + '0' + self.cmd.SPLIT + '0' + self.cmd.SPLIT + '0' + self.cmd.SPLIT + self.cmd.ENTER
        try:
            self.socket_command_send(s)
        except:
            pass


if __name__ == '__main__':
    app = QApplication(sys.argv)
    myshow = ESP32_MainWindow()
    myshow.show()
    sys.exit(app.exec_())


