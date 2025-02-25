// udp_server.c
#include <arpa/inet.h>
#include <mavlink/common/mavlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  int sockfd;
  char client_buffer[BUFFER_SIZE];
  uint8_t server_buffer[MAVLINK_MAX_PACKET_LEN];
  struct sockaddr_in servaddr, cliaddr;
  socklen_t len;

  // Create socket
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // Fill server information
  servaddr.sin_family = AF_INET;  // IPv4
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);

  // Bind the socket with the server address
  if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  len = sizeof(cliaddr);  // len is value/result

  int n = recvfrom(sockfd, (char *)client_buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
  client_buffer[n] = '\0';
  printf("Client : %s\n", client_buffer);

  // MAVLink message
  mavlink_message_t message;

  const uint8_t system_id = 42;
  const uint8_t base_mode = 0;
  const uint8_t custom_mode = 0;
  mavlink_msg_heartbeat_pack_chan(system_id, MAV_COMP_ID_PERIPHERAL, MAVLINK_COMM_0, &message, MAV_TYPE_GENERIC,
                                  MAV_AUTOPILOT_GENERIC, base_mode, custom_mode, MAV_STATE_STANDBY);

  const int mav_len = mavlink_msg_to_send_buffer(server_buffer, &message);

  sendto(sockfd, server_buffer, mav_len, MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
  printf("Sent Mavlink Heartbeat.\n");

  close(sockfd);
  return 0;
}
