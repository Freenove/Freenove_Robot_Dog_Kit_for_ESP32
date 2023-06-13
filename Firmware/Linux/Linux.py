import os
import sys

os.system("python3 esptool.py --port /dev/ttyUSB0 erase_flash")
os.system("python3 esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 2000000 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0xe000 ../firmware/ota_data_initial.bin 0x1000 ../firmware/bootloader/bootloader.bin 0x10000 ../firmware/esp32_idf_dog.bin 0x8000 ../firmware/partition_table/partition-table.bin")
