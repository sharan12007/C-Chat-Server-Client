#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <mutex>
#include <map>
#include <algorithm>

#pragma comment(lib, "WS2_32.lib")

using namespace std;

vector<SOCKET> Clients;
map<SOCKET, string> Cname;
mutex Mutex;  // Mutex for thread safety
bool running = true;

void handleClient(SOCKET inputClient) {
    char buffer[1024];
    int bytesReceived;

    // Notify other clients that a new client has connected
    {
        lock_guard<mutex> lock(Mutex);
        string welcomeMessage = Cname[inputClient] + " Connected.";
        cout << welcomeMessage << endl;

        for (const auto& client : Clients) {
            if (client != inputClient) {
                send(client, welcomeMessage.c_str(), welcomeMessage.size(), 0);
            }
        }
    }

    while ((bytesReceived = recv(inputClient, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';  // Null-terminate the string

        // Construct the message to send to other clients
        string totalMessage = Cname[inputClient] + ": " + buffer;

        lock_guard<mutex> lock(Mutex);
        for (const auto& client : Clients) {
            if (client != inputClient) {
                send(client, totalMessage.c_str(), totalMessage.size(), 0);
            }
        }
    }

    // Clean up on client disconnect
    closesocket(inputClient);

    {
        lock_guard<mutex> lock(Mutex);
        Clients.erase(remove(Clients.begin(), Clients.end(), inputClient), Clients.end());
        cout << Cname[inputClient] << " disconnected." << endl;
        Cname.erase(inputClient);  // Clean up client name
    }
}

void acceptClient(SOCKET serverSocket) {
    while (running) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == SOCKET_ERROR) {
            cout << "Error in accept: " << WSAGetLastError() << endl;
            continue;  // Continue accepting other clients
        }

        char getName[20] ={ 0 };  // Ensure buffer is initialized
        int bytesReceived = recv(clientSocket, getName, sizeof(getName) - 1, 0);
        if (bytesReceived > 0) {
            getName[bytesReceived] = '\0';  // Null-terminate the received name
            string name(getName);
            name.erase(remove(name.begin(), name.end(), '\n'), name.end()); // Clean up any newlines
            name.erase(remove(name.begin(), name.end(), '\r'), name.end()); // Clean up any carriage returns

            {
                lock_guard<mutex> lock(Mutex);
                Cname[clientSocket] = name;
                Clients.push_back(clientSocket);
            }

            thread(handleClient, clientSocket).detach();
        }
        else {
            cout << "Failed to receive name from client." << endl;
            closesocket(clientSocket);  // Close socket if name is not received
        }
    }
}

int main() {
    char IP[20];
    int portNo;

    cout << "====================== SERVER ========================" << endl;
    cout << "Enter IP: ";
    cin >> IP;
    cout << "Enter Port No: ";
    cin >> portNo;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "Error initializing Winsock: " << WSAGetLastError() << endl;
        return 1;
    }
    else {
        cout << "Server Status: Running" << endl;  // Updated status message
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Socket creation error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    SOCKADDR_IN bindID;
    bindID.sin_family = AF_INET;
    bindID.sin_port = htons(portNo);
    inet_pton(AF_INET, IP, &bindID.sin_addr);

    if (bind(serverSocket, (SOCKADDR*)&bindID, sizeof(bindID)) == SOCKET_ERROR) {
        cout << "Bind unsuccessful: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Listen unsuccessful: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    else {
        cout << "Listening for connections..." << endl;
    }

    thread acceptThread(acceptClient, serverSocket);
    acceptThread.join();

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
