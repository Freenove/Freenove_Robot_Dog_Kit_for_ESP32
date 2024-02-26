#该类封装了一些调用esptool的指令参数
class COMMAND:
    #Firmware COMMAND
    PYTHON           = "python"
    PYTHON3          = "python3"
    SPACE            = " "
    ESPTOOL          = "esptool.py"
    ERASE_FLASH      = "erase_flash"
    PORT             = "--port"
    BAUD             = "--baud"
    CHIP             = "--chip"
    ESP32            = "esp32"
    CONFIG           = "--before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB"
    OTA_DATA_INITAL  = "0xe000 ./firmware/ota_data_initial.bin"
    BOOTLOADER       = "0x1000 ./firmware/bootloader/bootloader.bin"
    FIRMWARE         = "0x10000 ./firmware/firmware.bin"
    ESP32_IDF_DOG    = "0x10000 ./firmware/ESP32_IDF_DOG.bin"
    PARTITION_TABLE  = "0x8000 ./firmware/partition_table/partition-table.bin"

    #CONFIG ROBOT COMMAND
    ACTION_UP_DOWN =      'A'
    ACTION_BODY_HEIGHT =  'B'
    ACTION_RGB =          'C'
    ACTION_BUZZER =       'D'
    ACTION_TWIST =        'E'
    ACTION_MOVE_ANY =     'F'
    ACTION_CAMERA =       'G'
    ACTION_ULTRASONIC =   'H'
    ACTION_GET_VOLTAGE =  'I'
    ACTION_CALIBRATE =    'J'
    ACTION_SET_NVS =      'K'
    ACTION_INSTALLATION = 'L'
    ACTION_AUTO_WALKING = 'M'
    ACTION_NETWORK =      'N'
    ACTION_DANCING =      'O'
    ACTION_SET_ROBOT =    'R'
    ACTION_TEST =         'T'
    ID_CHECK =            'W'
    SPLIT =               '#'
    ENTER =               '\r\n'

def __init__(self):
    pass


