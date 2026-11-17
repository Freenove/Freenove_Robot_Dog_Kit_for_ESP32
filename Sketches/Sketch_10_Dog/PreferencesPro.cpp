/**
 * @file PreferencesPro.cpp
 * @author zhentao lin@freenove (support@freenove.com)
 * @brief More powerful storage class, which can store any type of data.
 * @version v1.2.0
 * @date 2026-11-17
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "PreferencesPro.h"

PreferencesPro prefs = PreferencesPro();

PreferencesPro::PreferencesPro()
{
}

PreferencesPro::~PreferencesPro()
{
}
void nvs_clear_all_data()
{
	nvs_flash_erase(); // erase the NVS partition and...
	nvs_flash_init(); // initialize the NVS partition.
}
