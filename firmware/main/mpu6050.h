#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

#define MPU6050_ADDR 0x68

void mpu6050_init(void);
void mpu6050_read_accel_gyro(float *accel, float *gyro);

#endif // MPU6050_H
