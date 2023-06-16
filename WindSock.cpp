#include <cstdlib>
#include <stdio.h>
#include <algorithm>

#include <iostream>
#include <vector>
#include <string>

#include <iostream>

// Currently set to windows 7 (last good version... ;) )
#define _WIN32_WINNT   0x0601
#define WINVER         0x0601

//#include <winsock2.h>
#include <WS2tcpip.h>

#define SERVER_PORT  54000
#define SERVER_ADDR  "127.0.0.1"


void initiateServer(void);
void initiateClient(void);


int main() {
    
    //initiateServer();
    initiateClient();
    
    return 1;
}



void initiateServer(void) {
    
    // Initiate winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    
    int WSOK = WSAStartup(ver, &wsData);
    if (WSOK != 0) {
        std::cout << "Failed to initiate winsock" << std::endl;
        return;
    }
    
    // Create a socket
    SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == INVALID_SOCKET) {
        std::cout << "Failed to open a socket" << std::endl;
        WSACleanup();
        return;
    }
    
    // Bind to an IP address and a PORT
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(SERVER_PORT);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;
    
    bind(listener, (sockaddr*)&hint, sizeof(hint));
    listen(listener, SOMAXCONN);
    
    // Wait for a connection
    sockaddr_in client;
    int clientSz = sizeof(client);
    
    std::cout << "Listening on port: " << SERVER_PORT << std::endl << std::endl;
    
    // Listen for a connection
    SOCKET clientSocket = accept(listener, (sockaddr*)&client, &clientSz);
    
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    
    ZeroMemory(host, NI_MAXHOST);
    ZeroMemory(service, NI_MAXSERV);
    
    int nameInfo = getnameinfo((sockaddr*)&client, clientSz, host, NI_MAXHOST, service, NI_MAXSERV, 0);
    
    if (nameInfo == 0) {
        
        std::cout << "Connected" << std::endl;
        std::cout << host << std::endl;
        std::cout << service << std::endl;
    }// else {
        //inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        //std::cout << "Connected" << std::endl;
        //std::cout << host << std::endl;
        //std::cout << ntohs(client.sin_port) << std::endl;
    //}
    
    
    // Close socket
    closesocket(listener);
    
    char buffer[1024];
    ZeroMemory(buffer, sizeof(buffer));
    
    bool isActive = true;
    while(isActive) {
        
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        
        if (bytesReceived == SOCKET_ERROR) {
            std::cout << "Error in received data" << std::endl;
            break;
        }
        
        if (bytesReceived == 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }
        
        // Send back the data
        char sendBuffer[] = {'S','E','R','V','E','R'};
        send(clientSocket, sendBuffer, sizeof(sendBuffer) + 1, 0);
        
    }
    
    // Close the socket and shutdown winsock
    closesocket(clientSocket);
    
    WSACleanup();
    
}


void initiateClient(void) {
    
    std::string IPaddr = SERVER_ADDR;
    
    // Initiate winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    
    int WSOK = WSAStartup(ver, &wsData);
    if (WSOK != 0) {
        std::cout << "Failed to initiate winsock" << std::endl;
        return;
    }
    
    // Create a socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cout << "Failed to open a socket" << std::endl;
        WSACleanup();
        return;
    }
    
    // Bind to an IP address and a PORT
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(SERVER_PORT);
    unsigned long uipAddr = inet_addr(IPaddr.c_str());
    hint.sin_addr = (in_addr&)uipAddr;
    
    // Connect to the server
    int connectResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectResult)
    
    // Close the client socket
    closesocket(sock);
    
    WSACleanup();
    
}
















