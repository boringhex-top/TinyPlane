#ifndef FLIGHT_CONTROL_H
#define FLIGHT_CONTROL_H

#include <stdint.h>

extern float fc_roll, fc_pitch, fc_yaw;

void flight_control_init(void);
void flight_control_update(float ax, float ay, float az, float gx, float gy, float gz, float dt);
void flight_control_set_rc(int16_t throttle, int16_t turn, int16_t pitch);
void flight_control_get_telemetry(float *roll, float *pitch, float *yaw);

#endif // FLIGHT_CONTROL_H
