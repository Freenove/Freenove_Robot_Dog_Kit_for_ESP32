#ifndef _PREFERENCESPRO_h
#define _PREFERENCESPRO_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <nvs_flash.h>
#include <Preferences.h>

#define NMSPC_STORAGE "STORAGE"
#define KEY_SERVO_OFFSET "KEY_1"
#define KEY_WIFI_SSID "KEY_2"
#define KEY_WIFI_PSD "KEY_3"
#define KEY_LED_MODE "KEY_4"

//https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-reference/storage/nvs_flash.html
//The operation object of NVS is a key-value, where the key is an ASCII string, and the currently supported maximum key length is 15 characters.

class PreferencesPro : public Preferences {
public:
  PreferencesPro();

  template<typename T> const T &put(const char *key, T &t) {
    const char *ptr = (const char *)&t;
    //Serial.printf("put size: %d,\n", sizeof(t));
    putBytes(key, ptr, sizeof(t));
    return t;
  }

  template<typename T> T &get(const char *key, T &t) {
    char *ptr = (char *)&t;
    int len = getBytesLength(key);
    //char* buf = (char*)malloc(len);
    getBytes(key, ptr, len);
    //Serial.printf("get size: %d \n", len);
    return t;
  }

  ~PreferencesPro();
};

void nvs_clear_all_data();

#endif
