#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "hardware.h"
#include "mpu6050.h"
#include "wifi_ap.h"
#include "udp_server.h"
#include "flight_control.h"

static const char *TAG = "TINY_PLANE";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting Tiny Plane Flight Controller");

    hardware_init();
    mpu6050_init();

    wifi_init_softap();
    xTaskCreate(udp_server_task, "udp_server", 4096, (void*)0, 5, NULL);

    float accel[3], gyro[3];
    uint32_t last_time = xTaskGetTickCount();

    flight_control_init();

    while (1) {
        mpu6050_read_accel_gyro(accel, gyro);

        uint32_t current_time = xTaskGetTickCount();
        float dt = (current_time - last_time) * portTICK_PERIOD_MS / 1000.0f;
        if (dt <= 0.0f) dt = 0.01f; // fallback for precision issues
        last_time = current_time;

        flight_control_update(accel[0], accel[1], accel[2], gyro[0], gyro[1], gyro[2], dt);

        vTaskDelay(pdMS_TO_TICKS(10)); // 100Hz flight control loop
    }
}
