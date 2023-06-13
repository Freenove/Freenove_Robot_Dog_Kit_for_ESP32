/**
 * @file PreferencesPro.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief More powerful storage class, which can store any type of data.
 * @version v1.0.0
 * @date 2022-04-13
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
