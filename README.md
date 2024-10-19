# C++ Chat Server-Client Application

## Overview
This C++ Chat Server-Client application is a simple yet robust implementation of a real-time messaging system. It allows multiple clients to connect to a central server, facilitating text-based communication in a user-friendly manner. The application demonstrates fundamental concepts of socket programming, multithreading, and client-server architecture.

## Features
- **Real-Time Messaging:** Clients can send and receive messages instantly.
- **Multi-Client Support:** The server can handle multiple clients simultaneously using multithreading.
- **User Identification:** Each client can set a unique username for easier identification in conversations.
- **Message Broadcasting:** Messages sent by one client are broadcasted to all connected clients.
- **Simple Command Interface:** Users can easily send messages and view incoming messages in the console.

## Technologies Used
- C++
- POSIX Sockets
- Threading (C++11)
- Standard Template Library (STL)

## Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/username/cpp-chat-server-client.git
   ```
2. Navigate to the project directory:
   ```bash
   cd cpp-chat-server-client
   ```
3. Compile the server and client applications:
   ```bash
   g++ server.cpp -o server -pthread
   g++ client.cpp -o client -pthread
   ```
4. Run the server:
   ```bash
   ./server
   ```
5. Open multiple terminals to run clients:
   ```bash
   ./client
   ```

## Usage
- After starting the server, clients can connect by entering the server IP and port.
- Users can send messages by typing in the console and pressing Enter.
- To exit, type `exit`.

## Contribution
Contributions are welcome! Feel free to submit issues, pull requests, or suggestions for new features.

## License
This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgments
Thanks to the open-source community for their contributions and inspiration in building this application.
