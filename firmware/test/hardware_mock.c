// Mock hardware functions for unit testing on host
#include "../main/hardware.h"

int mocked_m1_pwm = 0;
int mocked_m2_pwm = 0;

void motor_set_pwm(int m1_duty, int m2_duty) {
    mocked_m1_pwm = m1_duty;
    mocked_m2_pwm = m2_duty;
}
