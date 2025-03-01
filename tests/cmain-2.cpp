#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

int main() {

// for the server we need to have an fd that stores what socket() returns.
// Then we also need the server address which is a sockaddr_in.
//

	//first declare a fd what the server can write to
	int server_fd, client_fd;

	//then have a struct of IPv4 for both server and client sockets
	struct sockaddr_in server_addr, client_addr;

	socklen_t client_addr_len = sizeof(client_addr);

	char buffer[1024];
	int port = 6667; // Default IRC port

	// socket - create an endpoint for communication
	// int socket(int domain, int type, int protocol);
	// Create the server socket
	// returns a file descriptor
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("socket");
		return 1;
	}

	// assign zeros to all the allocated memory to avoid touching
	// somewhere with gabbage data
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET; // Set the address family to IPv4
	server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to all available interfaces
	server_addr.sin_port = htons(port); // Convert the port to network byte order

	//The bind() function shall assign a local socket address address to a socket identified by descriptor socket that has no local socket address assigned. Sockets created with the socket() function are initially unnamed; they are identified only by their address family.
	// The bind() function takes the following arguments:
	// socket
	// Specifies the file descriptor of the socket to be bound.
	// address
	// Points to a sockaddr structure containing the address to be bound to the socket. The length and format of the address depend on the address family of the socket.
	// address_len
	// Specifies the length of the sockaddr structure pointed to by the address argument.
	// The socket specified by socket may require the process to have appropriate privileges to use the bind() function.
	// RETURN VALUE
	// Upon successful completion, bind() shall return 0; otherwise, -1 shall be returned and errno set to indicate the error.

	// Bind the socket to the address and port
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
		close(server_fd);
		return 1;
	}

	// Listen for incoming connections
	if (listen(server_fd, 5) < 0) {
		perror("listen");
		close(server_fd);
		return 1;
	}

	//memset(), bind(), listen(), accept()

	std::cout << "Server listening on port " << port << std::endl;

	// accept - accept a new connection on a socket
	// int accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
	// Accept a connection

	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_fd < 0) {
		perror("accept");
		close(server_fd);
		return 1;
	}

	std::cout << "Client connected" << std::endl;

	// Read data from the client
	ssize_t bytes_read;
	while (1){
		if (bytes_read < 0) {
			perror("read");
			close(client_fd);
			close(server_fd);
			return 1;
		}
		else {
			bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
			buffer[bytes_read] = '\0'; // Null-terminate the buffer
			std::cout << "Received message: " << buffer << std::endl;
		}
	}

	// Close the client and server sockets
	close(client_fd);
	close(server_fd);

	return 0;
}
