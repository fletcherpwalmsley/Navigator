// udp_client.c
#include <arpa/inet.h>
#include <mavlink/common/mavlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 14550
#define BUFFER_SIZE 1024

int main() {
  int sockfd;
  char buffer[BUFFER_SIZE];
  struct sockaddr_in servaddr;

  // Create socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  printf("Socket created successfully\n");
  memset(&servaddr, 0, sizeof(servaddr));

  // Fill server information
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Connect to server
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("Connection to server failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  printf("Connected to server\n");

  ssize_t n = recv(sockfd, buffer, sizeof(buffer), 0);

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

  send(sockfd, buffer, sizeof(buffer), 0);

  n = recv(sockfd, buffer, sizeof(buffer), 0);

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
