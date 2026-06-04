#include "flight_control.h"
#include "hardware.h"
#include <math.h>

float fc_roll = 0.0f;
float fc_pitch = 0.0f;
float fc_yaw = 0.0f;

static int16_t current_throttle = 0;
static int16_t current_turn = 0;
static int16_t current_pitch_rc = 0;

void flight_control_init(void) {
    fc_roll = 0.0f;
    fc_pitch = 0.0f;
    fc_yaw = 0.0f;
}

void flight_control_set_rc(int16_t throttle, int16_t turn, int16_t pitch) {
    current_throttle = throttle;
    current_turn = turn;
    current_pitch_rc = pitch;
}

void flight_control_update(float ax, float ay, float az, float gx, float gy, float gz, float dt) {
    // 1. Attitude Estimation (Simple Complementary Filter)
    float accel_roll  = atan2f(ay, az) * 180.0f / M_PI;
    float accel_pitch = atan2f(-ax, sqrtf(ay * ay + az * az)) * 180.0f / M_PI;

    // Filter coefficients
    float alpha = 0.98f;
    fc_roll  = alpha * (fc_roll + gx * dt) + (1.0f - alpha) * accel_roll;
    fc_pitch = alpha * (fc_pitch + gy * dt) + (1.0f - alpha) * accel_pitch;
    fc_yaw   += gz * dt; // Simple integration for yaw (will drift)

    // 2. Flight Control Logic (PID & Mixing)
    // For twin motor fixed wing:
    // Base throttle controls speed/altitude.
    // Differential thrust controls yaw/roll.

    // Convert rc to desired setpoints
    float desired_roll = (float)current_turn * 0.1f; // max +-50 degrees
    float roll_error = desired_roll - fc_roll;

    // Simple P controller for roll
    float kp = 2.0f;
    float roll_output = kp * roll_error;

    // Mixer
    int m1_pwm = 0;
    int m2_pwm = 0;

    if (current_throttle > 50) { // Deadband
        m1_pwm = current_throttle + (int)roll_output;
        m2_pwm = current_throttle - (int)roll_output;
    }

    // Constrain PWM to valid range (0-1023)
    if (m1_pwm > 1023) m1_pwm = 1023;
    if (m2_pwm > 1023) m2_pwm = 1023;
    if (m1_pwm < 0) m1_pwm = 0;
    if (m2_pwm < 0) m2_pwm = 0;

    motor_set_pwm(m1_pwm, m2_pwm);
}

void flight_control_get_telemetry(float *roll, float *pitch, float *yaw) {
    *roll = fc_roll;
    *pitch = fc_pitch;
    *yaw = fc_yaw;
}
