#include "mpu6050.h"
#include "hardware.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MPU6050";

#define MPU6050_SMPLRT_DIV      0x19
#define MPU6050_CONFIG          0x1A
#define MPU6050_GYRO_CONFIG     0x1B
#define MPU6050_ACCEL_CONFIG    0x1C
#define MPU6050_ACCEL_XOUT_H    0x3B
#define MPU6050_PWR_MGMT_1      0x6B

static esp_err_t mpu_write_byte(uint8_t reg_addr, uint8_t data) {
    uint8_t write_buf[2] = {reg_addr, data};
    return i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR, write_buf, sizeof(write_buf), 1000 / portTICK_PERIOD_MS);
}

void mpu6050_init(void) {
    esp_err_t err;
    // Wake up
    err = mpu_write_byte(MPU6050_PWR_MGMT_1, 0x00);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to wake up MPU6050");
        return;
    }
    // Set sample rate to 1kHz
    mpu_write_byte(MPU6050_SMPLRT_DIV, 0x07);
    // Configure DLPF
    mpu_write_byte(MPU6050_CONFIG, 0x06); // 5Hz
    // Configure Gyro (±500°/s)
    mpu_write_byte(MPU6050_GYRO_CONFIG, 0x08);
    // Configure Accel (±8g)
    mpu_write_byte(MPU6050_ACCEL_CONFIG, 0x10);
    ESP_LOGI(TAG, "MPU6050 initialized");
}

void mpu6050_read_accel_gyro(float *accel, float *gyro) {
    uint8_t data[14];
    esp_err_t err = i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDR, (uint8_t[]){MPU6050_ACCEL_XOUT_H}, 1, data, 14, 1000 / portTICK_PERIOD_MS);
    if (err == ESP_OK) {
        int16_t ax = (data[0] << 8) | data[1];
        int16_t ay = (data[2] << 8) | data[3];
        int16_t az = (data[4] << 8) | data[5];
        int16_t gx = (data[8] << 8) | data[9];
        int16_t gy = (data[10] << 8) | data[11];
        int16_t gz = (data[12] << 8) | data[13];

        // ±8g
        accel[0] = ax / 4096.0f;
        accel[1] = ay / 4096.0f;
        accel[2] = az / 4096.0f;

        // ±500°/s
        gyro[0] = gx / 65.5f;
        gyro[1] = gy / 65.5f;
        gyro[2] = gz / 65.5f;
    } else {
        ESP_LOGE(TAG, "Failed to read MPU6050");
        accel[0] = accel[1] = accel[2] = 0.0f;
        gyro[0] = gyro[1] = gyro[2] = 0.0f;
    }
}
