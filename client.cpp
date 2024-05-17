#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#define ERROR_C "CLIENT_ERROR: "
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1608
#define BUFFER_SIZE 1024
#define SERVER_CLOSE '#'

bool is_client_connect(const char* msg);

int main() {
    int client;
    struct sockaddr_in server_address;

    // Create client socket
    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        std::cerr << ERROR_C << "Socket creation failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_family = AF_INET;
    
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    std::cout << "\n => Client socket created. \n";

    // Connect to the server
    if (connect(client, (const struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        std::cerr << ERROR_C << "Connection failed." << std::endl;
        close(client);
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    std::cout << "=> Waiting for server confirmation...\n";
    recv(client, buffer, BUFFER_SIZE, 0);
    std::cout << "=> Connection established.\n"
              << "Enter '#' to close the connection.\n";

    while (true) {
        std::cout << "Client: ";
        std::cin.getline(buffer, BUFFER_SIZE);

        // Send message to the server
        send(client, buffer, strlen(buffer), 0);

        if (is_client_connect(buffer)) {
            break;
        }

        // Clear buffer before receiving server's response
        memset(buffer, 0, BUFFER_SIZE);

        // Receive message from server
        recv(client, buffer, BUFFER_SIZE, 0);
        std::cout << "Server: " << buffer << std::endl;

        if (is_client_connect(buffer)) {
            break;
        }
    }

    // Close the client socket
    close(client);
    std::cout << "Goodbye!!!" << std::endl;

    return 0;
}

bool is_client_connect(const char* msg) {
    for (int i = 0; i < strlen(msg); ++i) {
        if (msg[i] == SERVER_CLOSE) {
            return true;
        }
    }
    return false;
}
