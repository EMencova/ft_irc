
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>
#define MAXCLIENT 5

int main () {
	// server: fds, memset(), bind(), listen(), accept()
	// client:

	int server_fd;
	int active_fds = 1;
	unsigned short port = 6696;
	char BUFFER[1024];

// 	struct sockaddr_in {
// 	__uint8_t       sin_len;
// 	sa_family_t     sin_family;
// 	in_port_t       sin_port;
// 	struct  in_addr sin_addr;
// 	char            sin_zero[8];
// };
	struct sockaddr_in server_sock;
	struct sockaddr_in client_sock;
	socklen_t server_addr;
	socklen_t client_addr = sizeof(client_sock);

	if ((server_fd = socket(AF_INET, SOCK_STREAM , 0)) < 0){
		perror ("server_fd failed");
		// close (server_fd);
		// close (client_fd);
		return (1);
	}


	// IOmultiplexing
	// first set the server fd to non_blocking
	int server_flags = fcntl(server_fd, F_GETFL, 0);
	if (server_flags < 0){
		perror("get server flags failed");
		close (server_fd);
		return (-1);
	}
	if ((fcntl(server_fd, F_SETFL, server_flags | O_NONBLOCK)) < 0){
		perror("set server flags failed");
		close (server_fd);
		return (-1);
	}


	memset(&server_sock, 0, sizeof(server_sock));

	server_sock.sin_addr.s_addr = INADDR_ANY;
	server_sock.sin_family = AF_INET;
	server_sock.sin_port = htons(port);
	// server_sock.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_fd, (sockaddr *) &server_sock, sizeof(server_sock)) < 0){
		perror ("bind failed");
		close (server_fd);
		// close (client_fd);
		return (1);
	}

	if ((listen(server_fd, 5)) < 0) {
		perror ("listen failed");
		close (server_fd);
		// close (client_fd);
		return (1);
	}

	std::cout << "server listening on: " << port << std::endl;

	// struct pollfd {
	// 	int     fd;
	// 	short   events;
	// 	short   revents;
	// };

	struct pollfd clientfds[MAXCLIENT];
	for (int i = 0; i < MAXCLIENT; i++){
		clientfds[i].fd = -1;
		clientfds[i].events = 0;
	}

	clientfds[0].fd = server_fd;
	clientfds[0].events = POLLIN;

// for non_blocking io, there is no more accept()
	// if ((client_fd = accept(server_fd, (struct sockaddr *) &client_sock, &client_addr)) < 0){
	// 	perror ("client accept failed");
	// 	close (server_fd);
	// 	// close (client_fd);
	// 	return (1);
	// }


	while (true){
		// block indefinitely until an event occurs poll()
		if (poll(clientfds, active_fds, -1) < 0){
			perror("poll count");
			break;
		}

		// Check if the listening socket is ready (new connection)
		if (clientfds[0].revents & POLLIN){
			int new_client_fd = accept(server_fd, (struct sockaddr *) &client_sock, &client_addr);
			if (new_client_fd < 0){
				if (errno != EWOULDBLOCK && errno != EAGAIN)
					perror("clientfds accept failed");
			}
		else {
				// Set this new client socket to non-blocking mode
				int client_flags = fcntl(new_client_fd, F_GETFL, 0);
				if (client_flags < 0){
					perror("get client flags failed");
					close (new_client_fd);
				}
				else if ((fcntl(new_client_fd, F_SETFL, client_flags | O_NONBLOCK)) < 0){
					perror("set client flags failed");
					close (new_client_fd);
				}
				else {
					if (active_fds < MAXCLIENT){
						clientfds[active_fds].fd = new_client_fd;
						clientfds[active_fds].events = POLLIN;
						active_fds++;
						std::cout << "New client connected: FD " << new_client_fd << std::endl;
					}
					else {
						std::cout << "Max clients reached, rejecting new connection." << std::endl;
						close (new_client_fd);
					}
				}
			}
		}

		// Check the client sockets for incoming data

		for (int i = 1; i < active_fds; i++) {
			if (clientfds[i].fd == -1)
				continue;

			if (clientfds[i].revents & POLLIN) {
				ssize_t bytes_read = read(clientfds[i].fd, BUFFER, sizeof(BUFFER) - 1);
				if (bytes_read < 0) {
					if (errno != EWOULDBLOCK && errno != EAGAIN) {
						perror("read failed in client socket incoming data");
						close(clientfds[i].fd);
						clientfds[i].fd = -1;
					}
				} else if (bytes_read == 0) {
					// The client disconnected
					std::cout << "Client disconnected: FD " << clientfds[i].fd << std::endl;
					close(clientfds[i].fd);
					clientfds[i].fd = -1;
				} else {
					BUFFER[bytes_read] = '\0';
					std::cout << "Received from FD " << clientfds[i].fd << ": " << BUFFER << std::endl;
				}
			}
		}
		// Compact the active_fds array to remove closed sockets
		// Compact the fds array to remove closed sockets
		for (int i = 1; i < active_fds; i++) {
			if (clientfds[i].fd == -1) {
				for (int j = i; j < active_fds - 1; j++) {
					clientfds[j] = clientfds[j + 1];
				}
				active_fds--;
				i--; // re-check the new entry at this position
			}
		}
	}

	// Clean up: close all open sockets
	for (int i = 0; i < active_fds; i++) {
		if (clientfds[i].fd != -1) {
			close(clientfds[i].fd);
		}
	}
}
