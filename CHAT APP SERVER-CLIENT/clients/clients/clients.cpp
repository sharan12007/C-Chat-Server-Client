#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <string>

#pragma comment(lib, "WS2_32.lib")

using namespace std;

// Function to receive messages from the server
void receiveMessages(SOCKET sock) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            cout << "Disconnected from server." << endl;
            break;
        }
        buffer[bytesReceived] = '\0';  // Null-terminate the received message
        cout << buffer << endl;  // Display the received message
    }
}

int main() {
    cout << "====================== CLIENT ========================" << endl;
    char IP[20];
    int port;

    cout << "Enter IP: ";
    cin >> IP;
    cout << "Enter Port: ";
    cin >> port;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock: " << WSAGetLastError() << endl;
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Socket creation failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, IP, &serverAddr.sin_addr);  // Convert IP address from text to binary form

    // Attempt to connect to the server
    while (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Connection failed! Retrying in 2 seconds..." << endl;
        Sleep(2000);  // Wait before retrying
    }

    string username;
    cout << "Enter Name: ";
    cin >> username;
    send(sock, username.c_str(), username.size(), 0);  // Send the username to the server

    thread(receiveMessages, sock).detach();  // Start a thread to receive messages

    // Message input loop
    string message;
    cin.ignore();  // Ignore the newline left by previous input
    while (true) {
        cout << "> ";
        getline(cin, message);  // Use getline for multi-word messages
        if (message == "exit") break;
        send(sock, message.c_str(), message.size(), 0);  // Send message to server
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
