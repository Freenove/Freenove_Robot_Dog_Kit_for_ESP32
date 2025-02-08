@echo off

:start

echo.

esptool.exe --port COM3 erase_flash

echo.

esptool.exe --chip esp32 --port COM3 --baud 1000000 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 ../firmware/ota_data_initial.bin 0x1000 ../firmware/bootloader/bootloader.bin 0x10000 ../firmware/esp32_idf_dog.bin 0x8000 ../firmware/partition_table/partition-table.bin

echo.

pause

echo. 
