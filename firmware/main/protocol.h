#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define PACKET_HEADER 0xAA55

typedef enum {
    CMD_RC_CONTROL = 0x01,
    CMD_TELEMETRY  = 0x02
} cmd_type_t;

#pragma pack(push, 1)

// Packet Header
typedef struct {
    uint16_t header; // 0xAA55
    uint8_t  cmd;
    uint8_t  len;
} packet_header_t;

// RC Control Payload
typedef struct {
    int16_t throttle; // 0 to 1000
    int16_t turn;     // -500 to +500 (yaw/roll differential)
    int16_t pitch;    // -500 to +500 (not directly used for twin motor, but can adjust base throttle)
} rc_control_t;

// Telemetry Payload
typedef struct {
    float roll;
    float pitch;
    float yaw;
    uint16_t battery_mv;
} telemetry_t;

// Full Packet
typedef struct {
    packet_header_t hdr;
    union {
        rc_control_t rc;
        telemetry_t telem;
    } payload;
    uint8_t checksum;
} flight_packet_t;

#pragma pack(pop)

uint8_t calculate_checksum(uint8_t *data, uint8_t len);

#endif // PROTOCOL_H
