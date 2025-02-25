// udp_server.c
#include <arpa/inet.h>
#include <mavlink/common/mavlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 14550
#define BUFFER_SIZE 1024

int main() {
  int sockfd, new_socket;
  char client_buffer[BUFFER_SIZE];
  uint8_t server_buffer[MAVLINK_MAX_PACKET_LEN];
  struct sockaddr_in servaddr;
  int addrlen = sizeof(servaddr);
  socklen_t len;

  // Create socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));

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
  printf("Bind successful\n");

  // Listen for incoming connections
  if (listen(sockfd, 3) < 0) {
    perror("Listen failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  printf("Listening for connections...\n");

  // Accept a connection
  new_socket = accept(sockfd, (struct sockaddr *)&servaddr, (socklen_t *)&addrlen);
  if (new_socket < 0) {
    perror("Accept failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  printf("Connection accepted\n");

  // Send MAVLink message
  mavlink_message_t message;

  const uint8_t system_id = 42;
  const uint8_t base_mode = 0;
  const uint8_t custom_mode = 0;
  mavlink_msg_heartbeat_pack_chan(system_id, MAV_COMP_ID_PERIPHERAL, MAVLINK_COMM_0, &message, MAV_TYPE_GENERIC,
                                  MAV_AUTOPILOT_GENERIC, base_mode, custom_mode, MAV_STATE_STANDBY);

  const int mav_len = mavlink_msg_to_send_buffer(server_buffer, &message);

  send(new_socket, server_buffer, mav_len, 0);
  printf("Sent Mavlink Heartbeat.\n");

  close(sockfd);
  return 0;
}
