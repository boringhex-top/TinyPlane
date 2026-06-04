#include "unity.h"

extern void test_calculate_checksum(void);
extern void test_flight_control_mixing_no_throttle(void);
extern void test_flight_control_mixing_forward(void);
extern void test_flight_control_mixing_turn(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_calculate_checksum);
    RUN_TEST(test_flight_control_mixing_no_throttle);
    RUN_TEST(test_flight_control_mixing_forward);
    RUN_TEST(test_flight_control_mixing_turn);
    return UNITY_END();
}
