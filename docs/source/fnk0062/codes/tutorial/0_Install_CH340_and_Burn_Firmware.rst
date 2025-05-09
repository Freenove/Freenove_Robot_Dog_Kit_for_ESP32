##############################################################################
Chapter 0 Install CH340 and Burn Firmware
##############################################################################

Check Firmware
******************

:red:`Please note that the firmware has been burnt by default, so generally, you do not need to burn it again.`

:red:`Please follow the steps below to check whether firmware has been burnt:`

+-----------------------------------------------------------------------------------------------------------+
| 1.Plug the ESP32 to the expansion board.                                                                  |
|                                                                                                           |
| |Chapter00_00|                                                                                            |
|                                                                                                           |
| :red:`Pay attention to the orientation of the ESP32 and make sure it is plugged in correctly; otherwise,` |
|                                                                                                           |
| :red:`it may damage the robot.`                                                                           |
+-----------------------------------------------------------------------------------------------------------+
| 2.Connect ESP32 to your computer with a USB cable.                                                        |
|                                                                                                           |
| After connecting, you can see the yellow LED on ESP32 stay ON and the blue LED blink twice every second.  |
|                                                                                                           |
| Meanwhile, the buzzer makes 4 warning sounds to tell you the camera has not yet been installed and then   |
|                                                                                                           |
| a pleasant sound to indicate the finish of initialization.                                                |
|                                                                                                           |
| |Chapter00_01|                                                                                            |
|                                                                                                           |
| If the above phenomenas happen, it means the firmware has been burnt.                                     |
+-----------------------------------------------------------------------------------------------------------+

.. |Chapter00_00| image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_00.png
.. |Chapter00_01| image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_01.png

:red:`If the firmware has been burnt on your robot, please skip to Chapter 1.`

:red:`Otherwise, please continue with the following steps.`

Please send emails to us (support@freenove.com) if you have any questions regarding the robot.

Install CH340 Driver
****************************

.. raw:: html

   <iframe height="500" width="690" src="https://www.youtube.com/embed/DfqqGdL_9hc" frameborder="0" allowfullscreen></iframe>

The computer uploads codes to ESP32 via CH340, so we need to install CH340 driver on our computer before using.

1.	First, download the CH340 driver. Click http://www.wch-ic.com/search?q=CH340&t=downloads to download the appropriate one based on your operating system.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_02.png
    :align: center

If you would not like to download the installation package, you can open “:red:`Freenove_Robot_Dog_Kit_for_ESP32/CH340`”. We have prepared the installation package.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_03.png
    :align: center

Next, we will explain how to install CH340 on different operating systems including Windows, Mac OS and Linux.

Windows
=============================

Check the Installation of CH340
------------------------------------------

1.	Connect esp32 to your computer with a USB cable.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_04.png
    :align: center

2.	Right-click on “This PC” on your computer desktop and select “Manage”.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_05.png
    :align: center

3.	 Click on “Device Manager” on the left of the pop-up window, and then click on “Ports” on the right. If your computer has installed CH340 driver, you can see the port: USB-SERIAL CH340 (COMx).

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_06.png
    :align: center

If so, you can click here to move to the next step.

If CH340 (COMx) does not show on your computer, you need to install CH340 driver.

Install CH340 Driver
-----------------------------------------

4.	Open “:red:`Freenove_Robot_Dog_Kit_for_ESP32/CH340/Windows/`”. 

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_07.png
    :align: center

5.	Double click to run the file “ **CH341SER.EXE** ”, whose interface is as below:

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_08.png
    :align: center

6.	Make sure ESP32 has connected to your computer and then click “INSTALL”. Wait for the installation to finish.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_09.png
    :align: center

The following window indicates that the installation finishes.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_10.png
    :align: center

7.	After installation, open device manager again and you can see the port USB-SERIAL CH340 (COMx).

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_11.png
    :align: center

So far, CH340 has been installed. Close all dialog boxes.

Mac OS
====================================

Check the Installation of CH340
-------------------------------------

Connect esp32 to your computer with a USB cable.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_12.png
    :align: center

Open Terminal of Mac OS, and type in the command ls /dev/cu.usb*

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_13.png
    :align: center

If your Terminal prints the message similar to the above, then your computer has installed the CH340. You can click here to move to the next step.

Otherwise, please continue with the following the steps.

Install CH340
-----------------------------------

If you would not like to download the installation package, you can open “Freenove_Robot_Dog_Kit_for_ESP32/CH340”. We have prepared the installation package.

Open the folder “:red:`Freenove_Robot_Dog_Kit_for_ESP32/CH340/MAC/`”

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_14.png
    :align: center

Click Continue.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_15.png
    :align: center

Click Install and wait for it to finish.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_16.png
    :align: center

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_17.png
    :align: center

Restart your PC.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_18.png
    :align: center

If CH340 is still not installed after the above steps, please refer to the ReadMe.pdf to install.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_19.png
    :align: center

Burn the Firmware
*********************************

For this product, ESP32 has burned the required firmware by default. If your ESP32 does not have the firmware or the firmware does not work, please re-burn the firmware with the following steps.

We will explain respectively for Windows, Mac OS and Linux systems. 

Linux
=================================

Check the Installation of CH340
----------------------------------

Open the system terminal and type in the command: lsmod | grep usbserial. If your computer has installed the driver, you should see the following information:

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_20.png
    :align: center

If the driver has been installed, you can determine the port used by ESP32 to communicate with your computer in this way:

1.	When ESP32 is not connected to your computer, open system terminal and type in the command ls /dev/tty*

2.	Connect ESP32 to your computer with a USB cable and type in the command ls /dev/tty* again.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_21.png
    :align: center

Compare the results. As shown below, :red:`/dev/ttyUSB0` is the port that ESP32 communicates with your computer.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_22.png
    :align: center

If your computer has installed CH340, you can click here to skip to the next step.

Install CH340
--------------------------------------

If you connect the ESP32 to your computer but it does not detect /dev/ttyUSB0, then it has not installed CH340 yet.

Please follow the steps below to install CH340 driver.

We have prepared the installation package for you: “Freenove_Robot_Dog_Kit_for_ESP32/CH340/LINUX/”.

1.	Enter the folder on terminal: **cd Freenove_Robot_Dog_Kit_for_ESP32/CH340/Linux/**

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_23.png
    :align: center

2.	Unzip the installation package: unzip CH341SER_LINUX.ZIP

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_24.png
    :align: center

3.	Enter the unzipped folder and type in the command sudo make to compile and generate the file ch34x.ko.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_25.png
    :align: center

4.	Use the ls command to check the file. As you can see below, the ch34.ko has been generated under the current directory.

5.	Type in the command to upload the file to the system: **sudo make load**

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_26.png
    :align: center

So far, the ch340 driver has been installed.

Windows
===================================

First, connect ESP32 to your computer with a USB cable. 

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_27.png
    :align: center

Second, open “windows.bat” under the directory of :red:`Freenove_Robot_Dog_Kit_for_ESP32/Firmware/Windows` with txt editor, and modify the COMx in the file according to the port USB-SERIAL CH340 (COMx) on your computer.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_28.png
    :align: center

:red:`Note: Do NOT modify other contents.`

Third, save and close the file. Double-click it to run and wait for it to finish downloading.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_29.png
    :align: center

Mac OS
====================================

1.	Open Terminal on your computer and type in the command python3 --version to check whether python3 has been installed on your computer.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_30.png
    :align: center

If your computer has not yet installed python3, please type in the command to install: brew install python3

2.	Type in the command **pip3 --version** to check whether python3 has integrated with pip3. If it has not, please type in the command **curl https://bootstrap.pypa.io/get-pip.py | python3** to install. 

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_31.png
    :align: center

3.	Enter the command to install firmware-downloading tool: pip3 install esptool

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_32.png
    :align: center

4.	Check whether esptool has been installed: pip3 list

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_33.png
    :align: center

5.	Connect ESP32 to your computer with the USB cable.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_34.png
    :align: center

6.	Open Mac OS Terminal and type in the command to check whether ESP32 can be detected: 

.. code-block:: console
    
    ls /dev/cu.usb*

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_35.png
    :align: center

The port number may vary among different computers. Here we take “/dev/cu.usbserial-14130” as an example. Copy the serial number.

7.	Open mac.py under the directory of Freenove_Robot_Dog_Kit_for_ESP32/Firmware/MAC OS/ with txt editor, and modify the port number to that of your computer. Save it and exit.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_36.png
    :align: center

Note: Please make sure only the port number is changed and other information, including space is not changed; otherwise, the firmware may fail to burn.

8.	Enter the command: cd :red:`Freenove_Robot_Dog_Kit_for_ESP32/Firmware/MAC OS/` and python3 mac.py one by one to install the firmware to esp32.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_37.png
    :align: center

9.	Wait for it to finish.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_38.png
    :align: center

Linux
===================================

1.	Check whether your computer has installed python3. If it has, please skip to the next step.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_39.png
    :align: center

If it has not, please run the command to install.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_40.png
    :align: center

2.	Check whether pip3 has been installed on your computer. If it has, please move on to the next step. Otherwise, please run the following commands to install:

.. code-block:: console
    
    sudo apt update&

.. code-block:: console
    
    sudo apt install python3-pip

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_41.png
    :align: center

3.	Type in the command: pip3 install esptool

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_42.png
    :align: center

4.	Connect ESP32 to your computer with the USB cable.

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_43.png
    :align: center

5.	Enter the command **ls /dev/ttyUSB0** to check the port number. 

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_44.png
    :align: center

6.	Enter the command **sudo chmod 777 /dev/ttyUSB0**

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_45.png
    :align: center

.. note:: 
    
    The above command is to give permission to/dev/ttyUSB0. Without this, the code may fail to download.

7.	Enter the directory of :red:`Freenove_Robot_Dog_Kit_for_ESP32/Firmware/Linux` and enter the command **python3 linux.py**

.. image:: ../_static/imgs/0_Install_CH340_and_Burn_Firmware/Chapter00_46.png
    :align: center  

8.	Wait for it to finish download.