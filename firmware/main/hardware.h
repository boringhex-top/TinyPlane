#ifndef HARDWARE_H
#define HARDWARE_H

#ifndef UNIT_TEST_HOST
#include "driver/i2c.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#endif

// I2C Pins for MPU6050
#define I2C_MASTER_SCL_IO           8
#define I2C_MASTER_SDA_IO           9
#define I2C_MASTER_NUM              0
#define I2C_MASTER_FREQ_HZ          400000

// Motor PWM Pins
#define MOTOR1_PIN                  10 // DRV1
#define MOTOR2_PIN                  5  // DRV2

#ifndef UNIT_TEST_HOST
#define LEDC_TIMER                  LEDC_TIMER_0
#define LEDC_MODE                   LEDC_LOW_SPEED_MODE
#define LEDC_CH_M1                  LEDC_CHANNEL_0
#define LEDC_CH_M2                  LEDC_CHANNEL_1
#define LEDC_DUTY_RES               LEDC_TIMER_10_BIT
#define LEDC_FREQUENCY              5000 // 5kHz for brushed motors

// ADC Pins
#define ADC_UNIT                    ADC_UNIT_1
#define BAT_VOLT_ADC_CH             ADC_CHANNEL_0 // IO0
#define BAT_CURR_ADC_CH             ADC_CHANNEL_3 // IO3
#endif

void hardware_init(void);
void motor_set_pwm(int m1_duty, int m2_duty);
int read_battery_voltage(void);

#endif // HARDWARE_H
