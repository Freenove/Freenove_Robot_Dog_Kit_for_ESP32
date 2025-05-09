##############################################################################
Chapter 14 Camera Web Server
##############################################################################

In this section, we'll use ESP32's video function as an example to study.

Connect ESP32 using USB and check its IP address through serial monitor. Use web page to access IP address to obtain video and image data.

Sketch
********************************

Sketch_09_Camera
==============================

.. image:: ../_static/imgs/14_Camera_Web_Server/Chapter14_00.png
    :align: center

Before running the program, please modify your router's name and password in the box shown in the illustration above to make sure that your Sketch can compile and work successfully. 

If your Arduino IDE prompts you that your sketch is out of your project's storage space, compile the code again as configured below. 

.. image:: ../_static/imgs/14_Camera_Web_Server/Chapter14_01.png
    :align: center

Compile and upload codes to ESP32, open the serial monitor and set the baud rate to 115200, and the serial monitor will print out a network link address.

.. image:: ../_static/imgs/14_Camera_Web_Server/Chapter14_02.png
    :align: center

If your ESP32 has been in the process of connecting to router, but the information above has not been printed out, please re-check whether the router name and password have been entered correctly and press the reset key on ESP32-WROVER to wait for a successful connection prompt. 

Open a web browser, enter the IP address printed by the serial monitor in the address bar, and access it. 

Taking the Google browser as an example, here's what the browser prints out after successful access to ESP32's IP.

.. image:: ../_static/imgs/14_Camera_Web_Server/Chapter14_03.png
    :align: center

Click on Start Stream. The effect is shown in the image below.

.. image:: ../_static/imgs/14_Camera_Web_Server/Chapter14_04.png
    :align: center

.. note::

    If sketch compilation fails due to ESP32 support package, follow the steps of the image to open the CameraWebServer. This sketch is the same as described in the tutorial above.

.. image:: ../_static/imgs/14_Camera_Web_Server/Chapter14_05.png
    :align: center

The following is the main program code. You need include other code files in the same folder when write your own code.

.. literalinclude:: ../../../freenove_Kit/Sketches/Sketch_09_Camera/Sketch_09_Camera.ino
    :linenos:
    :language: c
    :dedent:

Add procedure files and API interface files related to ESP32 camera.

.. literalinclude:: ../../../freenove_Kit/Sketches/Sketch_09_Camera/Sketch_09_Camera.ino
    :linenos:
    :language: c
    :lines: 1-21
    :dedent:

Enter the name and password of the router

.. literalinclude:: ../../../freenove_Kit/Sketches/Sketch_09_Camera/Sketch_09_Camera.ino
    :linenos:
    :language: c
    :lines: 23-24
    :dedent:

Initialize serial port, set baud rate to 115200; open the debug and output function of the serial.

.. literalinclude:: ../../../freenove_Kit/Sketches/Sketch_09_Camera/Sketch_09_Camera.ino
    :linenos:
    :language: c
    :lines: 29-31
    :dedent:

Configure parameters including interface pins of the camera. Note: It is generally not recommended to change them.

.. literalinclude:: ../../../freenove_Kit/Sketches/Sketch_09_Camera/Sketch_09_Camera.ino
    :linenos:
    :language: c
    :lines: 33-53
    :dedent:

ESP32 connects to the router and prints a successful connection prompt. If it has not been successfully connected, press the reset key on the ESP32-WROVER.

.. literalinclude:: ../../../freenove_Kit/Sketches/Sketch_09_Camera/Sketch_09_Camera.ino
    :linenos:
    :language: c
    :lines: 94-101
    :dedent:

Open the video streams server function of the camera and print its IP address via serial port.

.. literalinclude:: ../../../freenove_Kit/Sketches/Sketch_09_Camera/Sketch_09_Camera.ino
    :linenos:
    :language: c
    :lines: 103-107
    :dedent:

Configure the display image information of the camera.

The set_vflip() function sets whether the image is flipped 180°, with 0 for no flip and 1 for flip 180°.

The set_brightness() function sets the brightness of the image, with values ranging from -2 to 2.

The set_saturation() function sets the color saturation of the image, with values ranging from -2 to 2.

.. literalinclude:: ../../../freenove_Kit/Sketches/Sketch_09_Camera/Sketch_09_Camera.ino
    :linenos:
    :language: c
    :lines: 79-85
    :dedent:

Modify the resolution and sharpness of the images captured by the camera. The sharpness ranges from 10 to 63, and the smaller the number, the sharper the picture. The larger the number, the blurrier the picture. Please refer to the table below.

.. literalinclude:: ../../../freenove_Kit/Sketches/Sketch_09_Camera/Sketch_09_Camera.ino
    :linenos:
    :language: c
    :lines: 58-60
    :dedent:

Reference
-------------------------------

+------------------+-----------+------------------+-----------+
| Image resolution | Sharpness | Image resolution | Sharpness |
+==================+===========+==================+===========+
| FRAMESIZE_QQVGA  | 160x120   | FRAMESIZE_VGA    | 640x480   |
+------------------+-----------+------------------+-----------+
| FRAMESIZE_QQVGA2 | 128x160   | FRAMESIZE_SVGA   | 800x600   |
+------------------+-----------+------------------+-----------+
| FRAMESIZE_QCIF   | 176x144   | FRAMESIZE_XGA    | 1024x768  |
+------------------+-----------+------------------+-----------+
| FRAMESIZE_HQVGA  | 240x176   | FRAMESIZE_SXGA   | 1280x1024 |
+------------------+-----------+------------------+-----------+
| FRAMESIZE_QVGA   | 320x240   | FRAMESIZE_UXGA   | 1600x1200 |
+------------------+-----------+------------------+-----------+
| FRAMESIZE_CIF    | 400x296   | FRAMESIZE_QXGA   | 2048x1536 |
+------------------+-----------+------------------+-----------+