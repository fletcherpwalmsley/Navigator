#include <arpa/inet.h>
#include <fcntl.h>
#include <mavlink/common/mavlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 14550
#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

/*
 * Make a socket non-blocking
 */
static int setnonblocking(int sockfd) {
  if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) == -1) {
    return -1;
  }
  return 0;
}

/*
 *  Send MAVLink Heartbeat to client
 */
void send_heartbeat(int client_fd) {
  //
  mavlink_message_t message;
  uint8_t server_buffer[MAVLINK_MAX_PACKET_LEN];

  const uint8_t system_id = 42;
  const uint8_t base_mode = 0;
  const uint8_t custom_mode = 0;
  mavlink_msg_heartbeat_pack_chan(system_id, MAV_COMP_ID_PERIPHERAL, MAVLINK_COMM_0, &message, MAV_TYPE_GENERIC,
                                  MAV_AUTOPILOT_GENERIC, base_mode, custom_mode, MAV_STATE_STANDBY);

  const int mav_len = mavlink_msg_to_send_buffer(server_buffer, &message);

  send(client_fd, server_buffer, mav_len, 0);
  printf("Sent Mavlink Heartbeat.\n");
}

int main() {
  int server_fd, new_socket, epoll_fd, event_count, i;
  struct sockaddr_in address;
  struct epoll_event event, events[MAX_EVENTS];
  int addrlen = sizeof(address);
  char buffer[BUFFER_SIZE] = {0};

  // Create socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }
  printf("Socket created successfully\n");

  // Define server address
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Bind socket to the address
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Bind failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
  printf("Bind successful\n");
  setnonblocking(server_fd);

  // Listen for incoming connections
  if (listen(server_fd, 3) < 0) {
    perror("Listen failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
  printf("Listening for connections...\n");

  // Create epoll instance
  epoll_fd = epoll_create1(0);
  if (epoll_fd == -1) {
    perror("Epoll creation failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  // Add server socket to epoll instance
  event.events = EPOLLIN;
  event.data.fd = server_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
    perror("Epoll control failed");
    close(server_fd);
    close(epoll_fd);
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Wait for events
    event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if (event_count == -1) {
      perror("Epoll wait failed");
      close(server_fd);
      close(epoll_fd);
      exit(EXIT_FAILURE);
    }

    for (i = 0; i < event_count; i++) {
      if (events[i].data.fd == server_fd) {
        // Accept new connection
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        setnonblocking(new_socket);
        if (new_socket == -1) {
          perror("Accept failed");
          continue;
        }
        printf("Connection accepted\n");

        // Add new socket to epoll instance
        event.events = EPOLLIN;
        event.data.fd = new_socket;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &event) == -1) {
          perror("Epoll control failed");
          close(new_socket);
          continue;
        }
        send_heartbeat(new_socket);
      } else {
        // Handle data from client
        int client_fd = events[i].data.fd;
        int valread = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (valread <= 0) {
          // Close connection if error or client disconnected
          if (valread == 0) {
            printf("Client disconnected\n");
          } else {
            perror("Receive failed");
          }
          close(client_fd);
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
        } else {
          buffer[valread] = '\0';
          printf("Received message: %s\n", buffer);

          // // Send response to client
          // if (send(client_fd, message, strlen(message), 0) < 0) {
          //   perror("Send failed");
          //   close(client_fd);
          //   epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
          // }
          send_heartbeat(client_fd);
        }
      }
    }
  }

  // Close the server socket and epoll instance
  close(server_fd);
  close(epoll_fd);
  return 0;
}
