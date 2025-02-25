// udp_client.c
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
  char buffer[BUFFER_SIZE];
  char *message = "Hello, Server!";
  struct sockaddr_in servaddr;

  // Create socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));

  // Fill server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = INADDR_ANY;

  int n, len;

  sendto(sockfd, (const char *)message, strlen(message), MSG_CONFIRM, (const struct sockaddr *)&servaddr,
         sizeof(servaddr));
  printf("Message sent.\n");

  n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);

  mavlink_message_t mavlink_message;
  mavlink_status_t status;
  mavlink_heartbeat_t heartbeat;

  for (int i = 0; i < n; ++i) {
    if (mavlink_parse_char(MAVLINK_COMM_0, buffer[i], &mavlink_message, &status) == 1) {
      switch (mavlink_message.msgid) {
        case MAVLINK_MSG_ID_HEARTBEAT:
          mavlink_msg_heartbeat_decode(&mavlink_message, &heartbeat);
          printf("Got heartbeat from ");
          switch (heartbeat.autopilot) {
            case MAV_AUTOPILOT_GENERIC:
              printf("generic");
              break;
            case MAV_AUTOPILOT_ARDUPILOTMEGA:
              printf("ArduPilot");
              break;
            case MAV_AUTOPILOT_PX4:
              printf("PX4");
              break;
            default:
              printf("other");
              break;
          }
          printf(" autopilot\n");
          break;
        default:
          printf("Unknown message\n");
          break;
      }
    }
  }

  close(sockfd);
  return 0;
}
