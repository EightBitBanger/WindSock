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





class WindSock {
    
public:
    
    WindSock(void);
    ~WindSock(void);
    
    void setAddress(std::string newAddress) {address = newAddress;}
    void setPort(unsigned int newPort)      {port = newPort;}
    
    
    /// Start a server listening for incoming connections.
    void startServerListener(void);
    
    /// Connect to a server who is listening incoming connections.
    void connectToServer(void);
    
    
    std::string  getAddress(void) {return address;}
    unsigned int getPort(void)    {return port;}
    
    
private:
    
    
    std::string address;
    unsigned int port;
    
    SOCKET mSocket;
    
};


WindSock::WindSock(void) {
    
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    
    int WSOK = WSAStartup(ver, &wsData);
    if (WSOK != 0) 
        return;
    
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket == INVALID_SOCKET) {
        WSACleanup();
        return;
    }
    
    return;
}

WindSock::~WindSock(void) {
    
    if (mSocket != INVALID_SOCKET) 
        closesocket(mSocket);
    
    WSACleanup();
}

void WindSock::startServerListener(void) {
    
    // Bind to an IP address and a PORT
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;
    
    bind(mSocket, (sockaddr*)&hint, sizeof(hint));
    listen(mSocket, SOMAXCONN);
    
    // Wait for a connection
    sockaddr_in client;
    int clientSz = sizeof(client);
    
    std::cout << "Listening on port: " << port << std::endl << std::endl;
    
    while(true) {
        // Listen for a connection
        /*SOCKET clientSocket = */accept(mSocket, (sockaddr*)&client, &clientSz);
        
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];
        
        ZeroMemory(host, NI_MAXHOST);
        ZeroMemory(service, NI_MAXSERV);
        
        /*int nameInfo = */getnameinfo((sockaddr*)&client, clientSz, host, NI_MAXHOST, service, NI_MAXSERV, 0);
        
        std::cout << "Connected" << std::endl;
        std::cout << host << std::endl;
        std::cout << service << std::endl << std::endl;
    }
}

void WindSock::connectToServer(void) {
    
    // Bind to an IP address and a PORT
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    unsigned long uipAddr = inet_addr(address.c_str());
    hint.sin_addr = (in_addr&)uipAddr;
    
    // Connect to the server
    /*int connectResult = */connect(mSocket, (sockaddr*)&hint, sizeof(hint));
    //if (connectResult) 
    
}





/*




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
    hint.sin_port = htons(57000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;
    
    bind(listener, (sockaddr*)&hint, sizeof(hint));
    listen(listener, SOMAXCONN);
    
    // Wait for a connection
    sockaddr_in client;
    int clientSz = sizeof(client);
    
    std::cout << "Listening on port: " << 57000 << std::endl << std::endl;
    
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
    
    
    
    
    
    std::string IPaddr = "127.0.0.1";
    
    // Bind to an IP address and a PORT
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(57000);
    unsigned long uipAddr = inet_addr(IPaddr.c_str());
    hint.sin_addr = (in_addr&)uipAddr;
    
    // Connect to the server
    int connectResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
    //if (connectResult) 
    
    // Close the client socket
    closesocket(sock);
    
    WSACleanup();
    
}




*/


