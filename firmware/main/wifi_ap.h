#ifndef WIFI_AP_H
#define WIFI_AP_H

#include "esp_err.h"

#define WIFI_SSID      "TinyPlane"
#define WIFI_PASS      "12345678"
#define WIFI_CHANNEL   1
#define MAX_STA_CONN   4

esp_err_t wifi_init_softap(void);

#endif // WIFI_AP_H
