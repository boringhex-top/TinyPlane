#include "hardware.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

static const char *TAG = "HARDWARE";

adc_oneshot_unit_handle_t adc1_handle;

static void i2c_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
    ESP_LOGI(TAG, "I2C initialized");
}

static void motor_init(void) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_ch_m1 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CH_M1,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = MOTOR1_PIN,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_ch_m1));

    ledc_channel_config_t ledc_ch_m2 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CH_M2,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = MOTOR2_PIN,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_ch_m2));

    ESP_LOGI(TAG, "Motors initialized");
}

static void adc_init(void) {
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, BAT_VOLT_ADC_CH, &config));
    // ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, BAT_CURR_ADC_CH, &config));
    ESP_LOGI(TAG, "ADC initialized");
}

void hardware_init(void) {
    i2c_init();
    motor_init();
    adc_init();
}

void motor_set_pwm(int m1_duty, int m2_duty) {
    // 10-bit duty: 0-1023
    if (m1_duty < 0) m1_duty = 0; if (m1_duty > 1023) m1_duty = 1023;
    if (m2_duty < 0) m2_duty = 0; if (m2_duty > 1023) m2_duty = 1023;

    ledc_set_duty(LEDC_MODE, LEDC_CH_M1, m1_duty);
    ledc_update_duty(LEDC_MODE, LEDC_CH_M1);

    ledc_set_duty(LEDC_MODE, LEDC_CH_M2, m2_duty);
    ledc_update_duty(LEDC_MODE, LEDC_CH_M2);
}

int read_battery_voltage(void) {
    int adc_raw;
    if (adc_oneshot_read(adc1_handle, BAT_VOLT_ADC_CH, &adc_raw) == ESP_OK) {
        // Need to calibrate and scale properly based on voltage divider
        return adc_raw;
    }
    return -1;
}
