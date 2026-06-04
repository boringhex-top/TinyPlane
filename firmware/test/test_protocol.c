#include "unity.h"
#include "protocol.h"

void setUp(void) {}
void tearDown(void) {}

void test_calculate_checksum(void) {
    uint8_t data[] = {0xAA, 0x55, 0x01, 0x06, 0x00, 0x00, 0x00};
    uint8_t expected_checksum = 0;
    for(int i=0; i < sizeof(data); i++) {
        expected_checksum ^= data[i];
    }

    uint8_t actual_checksum = calculate_checksum(data, sizeof(data));
    TEST_ASSERT_EQUAL_HEX8(expected_checksum, actual_checksum);
}
