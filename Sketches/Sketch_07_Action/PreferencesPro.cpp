#include "PreferencesPro.h"

PreferencesPro prefs = PreferencesPro();

PreferencesPro::PreferencesPro() {
}

PreferencesPro::~PreferencesPro() {
}
void nvs_clear_all_data() {
  nvs_flash_erase();  // erase the NVS partition and...
  nvs_flash_init();   // initialize the NVS partition.
}
