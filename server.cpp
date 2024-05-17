#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <algorithm>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1608
#define CLIENT_PORT 1603
#define SERVER_CLOSE '#'
#define ERROR_S "SERVER ERROR"
#define BUFFER_SIZE 1024

bool is_client_disconnect(const char*);
void handle_error(const char*);

int main() {
    int server, client;

    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    // Create server socket
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        handle_error("Socket creation failed");
    }

    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind server socket
    if (bind(server, (const sockaddr*) &server_address, sizeof(server_address)) < 0) {
        handle_error("Binding failed");
    }

    // Listen for incoming connections
    if (listen(server, 1) < 0) {
        handle_error("Listening failed");
    }

    std::cout << "SERVER: Listening for clients on port " << SERVER_PORT << std::endl;

    while (true) {
        // Accept incoming connection
        client = accept(server, (struct sockaddr*)&client_address, &client_address_len);
        if (client < 0) {
            handle_error("Accepting connection failed");
        }

        std::cout << "Client connected" << std::endl;

        // Send confirmation to the client that the connection is established
        const char* confirmation_message = "Connection established";
        send(client, confirmation_message, strlen(confirmation_message), 0);

        char buffer[BUFFER_SIZE];
        bool isExit = false;

        while (!isExit) {
            // Receive message from client
            ssize_t bytes_received = recv(client, buffer, BUFFER_SIZE, 0);
            if (bytes_received < 0) {
                handle_error("Error in receiving data from client");
            } else if (bytes_received == 0) {
                // Client disconnected
                std::cout << "Client disconnected" << std::endl;
                isExit = true;
                break;
            }

            buffer[bytes_received] = '\0'; // Null-terminate the received data
            std::cout << "Received from client: " << buffer << std::endl;

            if (is_client_disconnect(buffer)) {
                isExit = true;
                break;
            }

            // Sort the message
            std::sort(buffer, buffer + strlen(buffer));

            // Send sorted message back to client
            send(client, buffer, strlen(buffer), 0);
        }

        if (isExit) {
            std::cout << "\nClosing client connection..." << std::endl;
            close(client);
        }
    }

    // Close server socket 
    close(server); 
    std::cout << "Server closed" << std::endl;
    return 0;
}

bool is_client_disconnect(const char* msg) {
    return strchr(msg, SERVER_CLOSE) != nullptr;
}

void handle_error(const char* error_msg) {
    perror(error_msg);
    exit(EXIT_FAILURE);
}
