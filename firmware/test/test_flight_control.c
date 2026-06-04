#include "unity.h"
#include "flight_control.h"

extern int mocked_m1_pwm;
extern int mocked_m2_pwm;

void test_flight_control_mixing_no_throttle(void) {
    flight_control_init();
    flight_control_set_rc(0, 0, 0); // zero throttle

    // update with no movement
    flight_control_update(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.01f);

    // PWM should be zero when throttle is below deadband
    TEST_ASSERT_EQUAL_INT(0, mocked_m1_pwm);
    TEST_ASSERT_EQUAL_INT(0, mocked_m2_pwm);
}

void test_flight_control_mixing_forward(void) {
    flight_control_init();
    flight_control_set_rc(500, 0, 0); // 500 throttle, no turn

    // update with no movement (level)
    flight_control_update(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.01f);

    // PWM should be equal
    TEST_ASSERT_EQUAL_INT(500, mocked_m1_pwm);
    TEST_ASSERT_EQUAL_INT(500, mocked_m2_pwm);
}

void test_flight_control_mixing_turn(void) {
    flight_control_init();
    flight_control_set_rc(500, 200, 0); // 500 throttle, positive turn

    // update with no movement (level)
    flight_control_update(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.01f);

    // M1 should be higher, M2 should be lower based on P controller
    // desired_roll = 200 * 0.1 = 20
    // roll_error = 20 - 0 = 20
    // roll_output = 2.0 * 20 = 40
    // M1 = 500 + 40 = 540
    // M2 = 500 - 40 = 460
    TEST_ASSERT_EQUAL_INT(540, mocked_m1_pwm);
    TEST_ASSERT_EQUAL_INT(460, mocked_m2_pwm);
}
