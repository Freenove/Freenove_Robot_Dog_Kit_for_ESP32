# Sketch_10_Dog — Freenove ESP32 机器狗固件

基于 Freenove Robot Dog Kit for ESP32（FNK0062）的完整控制固件。支持 BLE / WiFi 遥控、摄像头推流、舵机步态、超声波避障、RGB 灯效与蜂鸣器等。

---

## 1. 开发环境与依赖

### 板型

- **开发板**：`ESP32-WROVER`（需带 PSRAM，摄像头与图像缓冲依赖 PSRAM）

### Arduino ESP32 SDK

| 项目 | 版本 / 说明 |
|------|-------------|
| Arduino IDE Boards Manager 包 | **esp32 by Espressif Systems** |
| 本工程实测 / 推荐版本 | **3.3.12**（Arduino-ESP32 3.x，底层 ESP-IDF 5.x） |
| Boards Manager URL | `https://espressif.github.io/arduino-esp32/package_esp32_index.json` |

### 需要安装的第三方库

| 库名 | 版本 | 安装方式 | 用途 |
|------|------|----------|------|
| **NimBLE-Arduino** | **2.5.2** | Arduino 库管理器搜索 `NimBLE` | BLE GATT 服务（比经典 Bluedroid 更省 RAM/Flash） |
| **Freenove WS2812 Lib for ESP32** | **2.0.2** | 库目录：`E:\Documents\Arduino\libraries\Freenove_WS2812_Lib_for_ESP32` | WS2812 RMT 驱动（兼容 IDF 4.x / 5.x） |

### 工程自带（无需再装）

以下源码已放在本工程目录内，随 sketch 一起编译：

| 组件 | 文件 | 说明 |
|------|------|------|
| Freenove PCA9685 | `Freenove_PCA9685.h/.cpp` | 16 路 PWM 舵机驱动（I2C） |

### ESP32 核心自带（随 SDK）

`Wire`、`WiFi`、`Preferences`、`EEPROM`、`esp_camera`、FreeRTOS 等，无需单独安装。

### 建议编译选项（Tools）

- Board: **ESP32 Wrover Module**
- Partition Scheme: **Huge APP (3MB No OTA/1MB SPIFFS)**
- PSRAM: **Enabled**

---

## 2. 文件说明

### 入口与公共层

| 文件 | 作用 |
|------|------|
| `Sketch_10_Dog.ino` | 程序入口：`setup` / `loop` / `loopSecondary`；串口 / BLE / WiFi 收包；消息入队与停止指令特殊处理 |
| `Public.h` / `Public.cpp` | 公共头文件与全局对象声明（队列、任务句柄、PCA9685、Preferences 等） |
| `RobotDefinitions.h` / `.cpp` | 硬件引脚、机身高度、软件版本、机器人名称等常量 |
| `BluetoothOrders.h` | 通信协议命令字定义（`A`~`W` 等动作码） |

### 通信与协议

| 文件 | 作用 |
|------|------|
| `BluetoothService.h` / `.cpp` | NimBLE GATT 服务：广播、收发、连接/断开时起立/趴下 |
| `CameraService.h` / `.cpp` | WiFi（AP/STA）、摄像头、命令口（5000）与图传口（8000）；自动识别 OV2640 / OV3660 / GC0308；无硬件 JPEG 时先采 RGB565 再 `frame2jpg` |
| `MessageParser.h` / `.cpp` | 将 `命令#参数#...\n` 文本解析为命令字与参数数组 |
| `MassageQueue.h` / `.cpp` | 环形消息队列模板 `DataQueue<T>`（运动 / 信息 / 发送 / 蜂鸣器） |

### 任务调度

| 文件 | 作用 |
|------|------|
| `TaskManager.h` / `.cpp` | 创建 / 挂起 / 恢复 FreeRTOS 任务 |
| `TaskCommandService.h` / `.cpp` | 非运动命令服务：ID 校验、RGB、蜂鸣器、电压、NVS、网络、摄像头参数等 |
| `TaskMotionService.h` / `.cpp` | 运动命令服务：安装位、校准、起立趴下、身高、扭转、任意方向走、舞蹈 |

### 运动与机械

| 文件 | 作用 |
|------|------|
| `Motion.h` / `.cpp` | 逆解（坐标→舵机角）、站立、任意方向步态、扭转、身高调节 |
| `DanceMovements.h` / `.cpp` | 打招呼、俯卧撑、伸懒腰、转身、坐下、跳舞等预设动作 |
| `Freenove_PCA9685.h` / `.cpp` | PCA9685 I2C PWM，驱动 12 路腿部舵机 |

### 外设驱动

| 文件 | 作用 |
|------|------|
| `BatteryPower.h` / `.cpp` | 电池 ADC 采样、电压监测与上报 |
| `Buzzer.h` / `.cpp` | 蜂鸣器 PWM 与旋律队列播放 |
| `NoteFrequency.h` | 音符频率表 |
| `UltrasonicRanging.h` / `.cpp` | HC-SR04 测距与自动行走避障逻辑 |
| `RGBLED_WS2812.h` / `.cpp` | 板载 4 颗 WS2812 灯效模式封装（依赖外部库 Freenove_WS2812_Lib_for_ESP32） |
| `BuiltInLed.h` / `.cpp` | 板载 LED（GPIO2）状态指示 |
| `TouchPad.h` / `.cpp` | 触摸按键检测与响应 |

### 存储

| 文件 | 作用 |
|------|------|
| `PreferencesPro.h` / `.cpp` | NVS 封装：舵机偏置、LED 配置等持久化读写 |

---

## 3. 机器狗逻辑关系

### 3.1 总体数据流

```
手机 App / 串口 / WiFi 客户端
        │
        ▼
  收包回调（按行 '\n' 组包）
  · onBleReceived
  · onWiFiCmdReceived
  · serialEventRun
        │
        ▼
  enterMessageQueue(msg)
        │
        ├─ 运动类命令（A/B/E/F/J/L/O…）──► mqMotion ──► task_MotionService
        │                                              │
        │                                              ▼
        │                                         MessageParser
        │                                              │
        │                                              ▼
        │                                    Motion / Dance / PCA9685
        │                                              │
        │                                              ▼
        │                                           12 路舵机
        │
        └─ 其它命令（C/D/G/H/I/K/N/R/W…）─► mqInfo ──► task_CommandService
                                                       │
                                                       ├─ RGB / 蜂鸣器 / 电压 / NVS
                                                       ├─ 摄像头 / WiFi 网络
                                                       └─ 状态回传 ──► mqTx ──► BLE notify
```

### 3.2 启动与任务结构

`setup()` 依次初始化：串口 → BLE → NVS → PCA9685（释放舵机）→ 读舵机偏置/LED 配置 → 摄像头/WiFi → ADC → 蜂鸣器 → 超声波 → RGB → 板载灯 → 触摸，然后：

1. 启动 **运动任务** `TASK_MOTION_SERVICE`（Core 1）
2. 启动 **副循环任务** `TASK_SECONDRAY` / `loopSecondary`（Core 1）
3. 播放开机旋律；若摄像头异常则提示；最后 `standUp()` 起立

主循环 `loop()`（Arduino 默认 Core 1）周期执行：

- `task_CommandService`：消费 `mqInfo`
- `task_BleUploadService`：把 `mqTx` 通过 BLE 发出
- `task_showBuiltInLed`
- `flushPendingMotionStop`：队列空闲时再真正入队停止指令
- 约 5 秒打印一次堆 / PSRAM 信息

`loopSecondary()` 周期执行：

- 电池监听、RGB 灯效、自动行走、蜂鸣器服务、触摸检测

摄像头任务由 `CameraService` 在需要时挂到 **Core 0**，与运动控制分离，减轻卡顿。

### 3.3 命令分流要点

| 命令字 | 含义 | 队列 |
|--------|------|------|
| `A` | 起立 / 趴下 | 运动 |
| `B` | 机身高度 | 运动 |
| `C` | RGB 灯 | 信息 |
| `D` | 蜂鸣器 | 信息 |
| `E` | 扭转姿态 | 运动 |
| `F` | 任意方向移动（含停止） | 运动（停止有特殊逻辑） |
| `G` | 摄像头 | 信息 |
| `H` | 超声波 | 信息 |
| `I` | 读电压 | 信息 |
| `J` | 腿部校准 | 运动 |
| `K` | NVS 读写/清除 | 信息 |
| `L` | 安装姿态 | 运动 |
| `M` | 自动行走开关 | 信息侧逻辑 + 超声波任务 |
| `N` | 网络（AP/STA） | 信息 |
| `O` | 舞蹈 | 运动 |
| `R` | 设置移动速度等 | 信息 |
| `W` | ID / 版本 / 控制器校验 | 信息 |

**停止指令优化**：`F#0#0#0#...` 不会立刻挤进运动队列，而是置 `motionStopPending`；等 `mqMotion` 清空后再入队，保证「停」永远是最后执行的动作，避免被旧步态覆盖。

运动队列满时会丢弃最旧消息（降延迟）；信息队列用 `enterForced`，尽量不丢重要控制/查询。

### 3.4 运动执行链

```
mqMotion 出队
    → MessageParser 解析
    → TaskMotionService 按命令分支
        → Motion：逆解 + 步态插值 / DanceMovements 预设动作
            → 更新各腿目标角（含 NVS 舵机偏置）
                → Freenove_PCA9685 写 PWM
                    → 舵机动作
```

### 3.5 传感与辅助逻辑

- **超声波**：测距；开启自动行走后，前方过近则停止/转向（`task_AutoWalking`）
- **电池**：ADC 采样电压，低电量可提示；与超声波共用相关 GPIO 时用互斥锁保护
- **触摸**：触发特定反馈或动作
- **BLE 连接/断开**：自动起立 / 趴下，并播放对应旋律

### 3.6 模块依赖简图

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
            PCA9685 ──► 舵机
```

---

## 4. 摄像头型号与画面配置

固件在 `CameraService::configureDetectedSensor()` 中按传感器 PID 自动识别型号，并设置默认的左右镜像 / 上下翻转。预览方向不对时，改这里的 `set_hmirror` / `set_vflip` 即可（`1` = 开启，`0` = 关闭）。

### 支持的型号与默认画面方向

| 传感器 | PID | 左右镜像 `set_hmirror` | 上下翻转 `set_vflip` | 像素格式 |
|--------|-----|------------------------|----------------------|----------|
| **OV2640** | `0x26` | `1`（开） | `1`（开） | 优先硬件 JPEG |
| **OV3660** | `0x3660` | `1`（开） | `0`（关） | 优先硬件 JPEG |
| **GC0308** | `0x9b` | `0`（关） | `0`（关） | 无硬件 JPEG → RGB565，推流前 `frame2jpg` |
| 其它未知 | — | `1`（开） | `0`（关） | 按驱动能力自动选择 |

### 如何调整画面方向

1. 打开 `CameraService.cpp`，找到 `configureDetectedSensor()`。
2. 按串口打印的 `Camera sensor PID: 0x...` / `Detected OVxxxx` 确认当前型号。
3. 修改对应分支：
   - **左右颠倒**（镜像）：改 `s->set_hmirror(s, 0/1);`
   - **上下颠倒**：改 `s->set_vflip(s, 0/1);`
4. 重新编译上传。

示例（OV2640 若觉得上下反了，把 `set_vflip` 从 `1` 改成 `0`）：

```cpp
if (pid == OV2640_PID)
{
    s->set_hmirror(s, 1);  // 左右镜像：1=开，0=关
    s->set_vflip(s, 0);    // 上下翻转：1=开，0=关
}
```

同函数内还会设置亮度 / 饱和度 / 曝光补偿（`set_brightness`、`set_saturation`、`set_ae_level`），可按实际画面再微调。

---

## 快速开始

1. 安装 Arduino IDE，配置 ESP32 板支持包 **3.3.12**
2. 安装 **NimBLE-Arduino 2.5.2**，并确保 **Freenove WS2812 Lib for ESP32 2.0.2** 已在 Arduino 库目录
3. 打开本目录下 `Sketch_10_Dog.ino`
4. 选择板型 **ESP32 Wrover Module**，开启 PSRAM，编译上传
5. 首次使用请先完成舵机校准（协议 `J`），再进行遥控行走

如有问题可参考 Freenove 官方仓库与文档：

- https://github.com/Freenove/Freenove_Robot_Dog_Kit_for_ESP32
- https://docs.freenove.com/projects/fnk0062/
