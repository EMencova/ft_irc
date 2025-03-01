
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>

int main () {
	// server: fds, memset(), bind(), listen(), accept()
	// client:

	int server_fd;
	int client_fd;
	unsigned short port = 6697;
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
		close (server_fd);
		close (client_fd);
		return (1);
	}

	memset(&server_sock, 0, sizeof(server_sock));

	server_sock.sin_addr.s_addr = INADDR_ANY;
	server_sock.sin_family = AF_INET;
	server_sock.sin_port = htons(port);
	// server_sock.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(server_fd, (sockaddr *) &server_sock, sizeof(server_sock)) < 0){
		perror ("bind failed");
		close (server_fd);
		close (client_fd);
		return (1);
	}

	if ((listen(server_fd, 5)) < 0) {
		perror ("listen failed");
		close (server_fd);
		close (client_fd);
		return (1);
	}

	std::cout << "server listening on: " << port << std::endl;

	if ((client_fd = accept(server_fd, (struct sockaddr *) &client_sock, &client_addr)) < 0){
		perror ("client accept failed");
		close (server_fd);
		close (client_fd);
		return (1);
	}

	ssize_t bytes_read;
	while (1){
		bytes_read = read(client_fd, BUFFER, sizeof(BUFFER) - 1);
		if (bytes_read < 0) {
			perror ("read failed");
			close (server_fd);
			close (client_fd);
			return (1);
		}
		else {
			BUFFER[bytes_read] = '\0';
			std::cout << "Received: " << BUFFER << '\r';
		}
	}
	close (server_fd);
	close (client_fd);
	return (1);
}
