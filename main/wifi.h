/*
 * wifi.h
 *
 *  Created on: Jun 1, 2021
 *      Author: Caliphax
 */

#ifndef MAIN_WIFI_H_
#define MAIN_WIFI_H_

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

esp_err_t wifi_connect_with_hostname( char* ssid, char* password, char* hostname );

#endif /* MAIN_WIFI_H_ */
