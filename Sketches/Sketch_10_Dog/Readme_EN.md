# Sketch_10_Dog — Freenove ESP32 Robot Dog Firmware

Full control firmware for the Freenove Robot Dog Kit for ESP32 (FNK0062). Supports BLE / WiFi remote control, camera streaming, servo gaits, ultrasonic obstacle avoidance, RGB lighting, buzzer, and more.

---

## 1. Development Environment & Dependencies

### Board

- **Board**: `ESP32-WROVER` (PSRAM required; camera and frame buffers depend on PSRAM)

### Arduino ESP32 SDK

| Item | Version / Notes |
|------|-----------------|
| Arduino IDE Boards Manager package | **esp32 by Espressif Systems** |
| Tested / recommended version | **3.3.12** (Arduino-ESP32 3.x, based on ESP-IDF 5.x) |
| Boards Manager URL | `https://espressif.github.io/arduino-esp32/package_esp32_index.json` |

### Third-party libraries to install

| Library | Version | How to install | Purpose |
|---------|---------|----------------|---------|
| **NimBLE-Arduino** | **2.5.2** | Arduino Library Manager, search `NimBLE` | BLE GATT service (uses less RAM/Flash than classic Bluedroid) |
| **Freenove WS2812 Lib for ESP32** | **2.0.2** | Arduino libraries folder (e.g. `Documents/Arduino/libraries/Freenove_WS2812_Lib_for_ESP32`) | WS2812 RMT driver (compatible with IDF 4.x / 5.x) |

### Bundled with this project (no extra install)

The following sources live in this sketch folder and compile with the project:

| Component | Files | Description |
|-----------|-------|-------------|
| Freenove PCA9685 | `Freenove_PCA9685.h/.cpp` | 16-channel PWM servo driver (I2C) |

### Built into the ESP32 core (with the SDK)

`Wire`, `WiFi`, `Preferences`, `EEPROM`, `esp_camera`, FreeRTOS, etc. — no separate install needed.

### Suggested Tools settings

- Board: **ESP32 Wrover Module**
- Partition Scheme: **Huge APP (3MB No OTA/1MB SPIFFS)**
- PSRAM: **Enabled**

---

## 2. File Overview

### Entry & common layer

| File | Role |
|------|------|
| `Sketch_10_Dog.ino` | Program entry: `setup` / `loop` / `loopSecondary`; Serial / BLE / WiFi RX; message enqueue and special stop handling |
| `Public.h` / `Public.cpp` | Shared headers and global objects (queues, task handles, PCA9685, Preferences, etc.) |
| `RobotDefinitions.h` / `.cpp` | Hardware pins, body height, software version, robot name, and other constants |
| `BluetoothOrders.h` | Protocol command characters (`A`–`W` action codes) |

### Communication & protocol

| File | Role |
|------|------|
| `BluetoothService.h` / `.cpp` | NimBLE GATT service: advertising, TX/RX, stand up / lie down on connect / disconnect |
| `CameraService.h` / `.cpp` | WiFi (AP/STA), camera, command port (5000) and stream port (8000); auto-detect OV2640 / OV3660 / GC0308; sensors without JPEG use RGB565 + `frame2jpg` |
| `MessageParser.h` / `.cpp` | Parses `cmd#param#...\n` text into a command character and parameter array |
| `MassageQueue.h` / `.cpp` | Ring queue template `DataQueue<T>` (motion / info / TX / buzzer) |

### Task scheduling

| File | Role |
|------|------|
| `TaskManager.h` / `.cpp` | Create / suspend / resume FreeRTOS tasks |
| `TaskCommandService.h` / `.cpp` | Non-motion commands: ID check, RGB, buzzer, voltage, NVS, network, camera settings, etc. |
| `TaskMotionService.h` / `.cpp` | Motion commands: install pose, calibration, stand/sit, body height, twist, omnidirectional walk, dance |

### Motion & mechanics

| File | Role |
|------|------|
| `Motion.h` / `.cpp` | Inverse kinematics (coords → servo angles), standing, omnidirectional gait, twist, height control |
| `DanceMovements.h` / `.cpp` | Preset moves: say hello, push-up, stretch, turn around, sit down, dance, etc. |
| `Freenove_PCA9685.h` / `.cpp` | PCA9685 I2C PWM driving 12 leg servos |

### Peripheral drivers

| File | Role |
|------|------|
| `BatteryPower.h` / `.cpp` | Battery ADC sampling, voltage monitoring and reporting |
| `Buzzer.h` / `.cpp` | Buzzer PWM and melody queue playback |
| `NoteFrequency.h` | Note frequency table |
| `UltrasonicRanging.h` / `.cpp` | HC-SR04 ranging and auto-walk obstacle avoidance |
| `RGBLED_WS2812.h` / `.cpp` | On-board 4× WS2812 effect modes (depends on external Freenove_WS2812_Lib_for_ESP32) |
| `BuiltInLed.h` / `.cpp` | On-board LED (GPIO2) status indicator |
| `TouchPad.h` / `.cpp` | Touch pad detection and response |

### Storage

| File | Role |
|------|------|
| `PreferencesPro.h` / `.cpp` | NVS wrapper: persistent servo offsets, LED config, etc. |

---

## 3. Robot Dog Logic

### 3.1 Overall data flow

```
Phone App / Serial / WiFi client
        │
        ▼
  RX callbacks (assemble lines by '\n')
  · onBleReceived
  · onWiFiCmdReceived
  · serialEventRun
        │
        ▼
  enterMessageQueue(msg)
        │
        ├─ Motion cmds (A/B/E/F/J/L/O…) ──► mqMotion ──► task_MotionService
        │                                              │
        │                                              ▼
        │                                         MessageParser
        │                                              │
        │                                              ▼
        │                                    Motion / Dance / PCA9685
        │                                              │
        │                                              ▼
        │                                           12 servos
        │
        └─ Other cmds (C/D/G/H/I/K/N/R/W…) ─► mqInfo ──► task_CommandService
                                                       │
                                                       ├─ RGB / buzzer / voltage / NVS
                                                       ├─ Camera / WiFi network
                                                       └─ Status TX ──► mqTx ──► BLE notify
```

### 3.2 Startup & task structure

`setup()` initializes in order: Serial → BLE → NVS → PCA9685 (release servos) → load servo offsets / LED config → camera / WiFi → ADC → buzzer → ultrasonic → RGB → on-board LED → touch, then:

1. Start the **motion task** `TASK_MOTION_SERVICE` (Core 1)
2. Start the **secondary loop** `TASK_SECONDRAY` / `loopSecondary` (Core 1)
3. Play power-up melody; warn if the camera failed; finally `standUp()`

Main loop `loop()` (Arduino default Core 1) periodically runs:

- `task_CommandService`: consume `mqInfo`
- `task_BleUploadService`: send `mqTx` over BLE
- `task_showBuiltInLed`
- `flushPendingMotionStop`: enqueue the real stop only when the motion queue is empty
- Heap / PSRAM stats about every 5 seconds

`loopSecondary()` periodically runs:

- Battery monitor, RGB effects, auto-walk, buzzer service, touch detection

Camera work is placed on **Core 0** by `CameraService` when needed, separate from motion control to reduce stalls.

### 3.3 Command routing

| Cmd | Meaning | Queue |
|-----|---------|-------|
| `A` | Stand up / lie down | Motion |
| `B` | Body height | Motion |
| `C` | RGB LEDs | Info |
| `D` | Buzzer | Info |
| `E` | Twist pose | Motion |
| `F` | Omnidirectional move (incl. stop) | Motion (stop has special handling) |
| `G` | Camera | Info |
| `H` | Ultrasonic | Info |
| `I` | Read voltage | Info |
| `J` | Leg calibration | Motion |
| `K` | NVS read/write/clear | Info |
| `L` | Installation pose | Motion |
| `M` | Auto-walk enable | Info-side logic + ultrasonic task |
| `N` | Network (AP/STA) | Info |
| `O` | Dance | Motion |
| `R` | Set move speed, etc. | Info |
| `W` | ID / version / controller check | Info |

**Stop-command optimization**: `F#0#0#0#...` is not pushed into the motion queue immediately. It sets `motionStopPending` and is enqueued only after `mqMotion` is empty, so “stop” is always the last motion action and is not overwritten by older gait commands.

When the motion queue is full, the oldest message is dropped (lower latency). The info queue uses `enterForced` so important control / query messages are less likely to be lost.

### 3.4 Motion execution chain

```
Dequeue mqMotion
    → MessageParser
    → TaskMotionService branches by command
        → Motion: IK + gait interpolation / DanceMovements presets
            → Update each leg’s target angles (with NVS servo offsets)
                → Freenove_PCA9685 write PWM
                    → Servos move
```

### 3.5 Sensing & helpers

- **Ultrasonic**: ranging; with auto-walk enabled, stop / turn when something is too close ahead (`task_AutoWalking`)
- **Battery**: ADC voltage sampling, low-battery indication; mutex protects shared GPIO with ultrasonic when needed
- **Touch**: triggers feedback or actions
- **BLE connect / disconnect**: auto stand up / lie down and play the matching melody

### 3.6 Module dependency sketch

```
                    Sketch_10_Dog.ino
                            │
                     ┌──────┴──────┐
                     ▼             ▼
              BluetoothService  CameraService(WiFi+Cam)
                     │             │
                     └──────┬──────┘
                            ▼
                   enterMessageQueue
                     /          \
              mqMotion         mqInfo
                 │               │
        TaskMotionService  TaskCommandService
                 │               │
           Motion/Dance     RGB/Buzzer/NVS/...
                 │
            PCA9685 ──► servos
```

---

## 4. Camera models & image orientation

The firmware auto-detects the sensor PID in `CameraService::configureDetectedSensor()` and applies default horizontal mirror / vertical flip. If the preview looks upside-down or mirrored wrong, change `set_hmirror` / `set_vflip` there (`1` = on, `0` = off).

### Supported sensors and default orientation

| Sensor | PID | H-mirror `set_hmirror` | V-flip `set_vflip` | Pixel format |
|--------|-----|------------------------|--------------------|--------------|
| **OV2640** | `0x26` | `1` (on) | `1` (on) | Hardware JPEG preferred |
| **OV3660** | `0x3660` | `1` (on) | `0` (off) | Hardware JPEG preferred |
| **GC0308** | `0x9b` | `0` (off) | `0` (off) | No hardware JPEG → RGB565, then `frame2jpg` before streaming |
| Unknown | — | `1` (on) | `0` (off) | Chosen by driver capability |

### How to adjust orientation

1. Open `CameraService.cpp` and find `configureDetectedSensor()`.
2. Confirm the model from Serial: `Camera sensor PID: 0x...` / `Detected OVxxxx`.
3. Edit the matching branch:
   - **Left-right mirrored**: change `s->set_hmirror(s, 0/1);`
   - **Upside-down**: change `s->set_vflip(s, 0/1);`
4. Rebuild and upload.

Example (OV2640 upside-down — set `set_vflip` from `1` to `0`):

```cpp
if (pid == OV2640_PID)
{
    s->set_hmirror(s, 1);  // horizontal mirror: 1=on, 0=off
    s->set_vflip(s, 0);    // vertical flip: 1=on, 0=off
}
```

The same function also sets brightness / saturation / AE level (`set_brightness`, `set_saturation`, `set_ae_level`); tweak those if the image looks too dark or oversaturated.

---

## Quick start

1. Install Arduino IDE and ESP32 board package **3.3.12**
2. Install **NimBLE-Arduino 2.5.2**, and make sure **Freenove WS2812 Lib for ESP32 2.0.2** is in your Arduino libraries folder
3. Open `Sketch_10_Dog.ino` in this directory
4. Select **ESP32 Wrover Module**, enable PSRAM, then build and upload
5. Calibrate the servos first (protocol `J`) before remote walking

For more help, see Freenove’s official repo and docs:

- https://github.com/Freenove/Freenove_Robot_Dog_Kit_for_ESP32
- https://docs.freenove.com/projects/fnk0062/
