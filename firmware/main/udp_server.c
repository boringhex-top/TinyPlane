#include "udp_server.h"
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "protocol.h"
#include "flight_control.h"

static const char *TAG = "UDP_SERVER";

void udp_server_task(void *pvParameters)
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family = (int)AF_INET;
    int ip_protocol = 0;
    struct sockaddr_in6 dest_addr;

    while (1) {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(UDP_PORT);
        ip_protocol = IPPROTO_IP;

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        }
        ESP_LOGI(TAG, "Socket bound, port %d", UDP_PORT);

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t socklen = sizeof(source_addr);

        while (1) {
            ESP_LOGD(TAG, "Waiting for data");
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                // Get the sender's ip address as string
                if (source_addr.ss_family == PF_INET) {
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
                }
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGD(TAG, "Received %d bytes from %s:", len, addr_str);

                // Protocol Parsing
                if (len >= sizeof(packet_header_t)) {
                    flight_packet_t *pkt = (flight_packet_t *)rx_buffer;
                    if (pkt->hdr.header == PACKET_HEADER) {
                        uint8_t expected_len = sizeof(packet_header_t) + pkt->hdr.len + 1; // +1 for checksum
                        if (len >= expected_len) {
                            uint8_t calc_chk = calculate_checksum((uint8_t *)rx_buffer, expected_len - 1);
                            if (calc_chk == rx_buffer[expected_len - 1]) {
                                if (pkt->hdr.cmd == CMD_RC_CONTROL && pkt->hdr.len == sizeof(rc_control_t)) {
                                    flight_control_set_rc(pkt->payload.rc.throttle, pkt->payload.rc.turn, pkt->payload.rc.pitch);
                                }
                            } else {
                                ESP_LOGW(TAG, "Checksum mismatch");
                            }
                        }
                    }
                }

                // Send Telemetry Response (could be at lower rate, but sending every receive for now)
                flight_packet_t telem_pkt;
                telem_pkt.hdr.header = PACKET_HEADER;
                telem_pkt.hdr.cmd = CMD_TELEMETRY;
                telem_pkt.hdr.len = sizeof(telemetry_t);

                flight_control_get_telemetry(&telem_pkt.payload.telem.roll,
                                             &telem_pkt.payload.telem.pitch,
                                             &telem_pkt.payload.telem.yaw);
                telem_pkt.payload.telem.battery_mv = 3700; // Mock battery voltage for now

                uint8_t *telem_ptr = (uint8_t *)&telem_pkt;
                telem_pkt.checksum = calculate_checksum(telem_ptr, sizeof(packet_header_t) + sizeof(telemetry_t));

                int err = sendto(sock, telem_ptr, sizeof(packet_header_t) + sizeof(telemetry_t) + 1, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
                if (err < 0) {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    break;
                }
            }
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}
