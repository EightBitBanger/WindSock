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
    
    std::string  GetHost(void)    {return host;}
    SOCKET       GetSocket(void)  {return mSocket;}
    
    
    // Server
    //
    
    /// Start a server listening for incoming connections.
    int InitiateServer(unsigned int portNumber);
    
    /// Check for an incoming connection request.
    int CheckIncomingConnections(void);
    
    /// Check for incoming messages from any connected client.
    int CheckIncomingMessages(char* buffer, unsigned int bufferSize);
    
    
    // Client
    //
    
    /// Connect to a server.
    int ConnectToServer(std::string address, unsigned int port);
    
    /// Disconnect from a server.
    int DisconnectFromServer(SOCKET socket);
    
    
    // Messaging
    //
    
    /// Send a message
    void MessageSend(SOCKET socket, char* buffer, unsigned int bufferSize);
    
    /// Receive a message
    int MessageReceive(SOCKET socket, char* buffer, unsigned int bufferSize);
    
    
private:
    
    bool isConnected;
    
    std::string host;
    
    SOCKET mSocket;
    
    std::vector<std::string> hostList;
    std::vector<SOCKET>      socketList;
    
};


WindSock::WindSock(void) :
    
    isConnected(false),
    
    host(""),
    
    mSocket(0)
{
    
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
    
}

WindSock::~WindSock(void) {
    
    if (mSocket != INVALID_SOCKET) 
        closesocket(mSocket);
    
    WSACleanup();
}

int WindSock::InitiateServer(unsigned int port) {
    
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;
    
    bind(mSocket, (sockaddr*)&hint, sizeof(hint));
    listen(mSocket, SOMAXCONN);
    
    // Non blocking socket
    u_long sockMode = 1;
    ioctlsocket(mSocket, FIONBIO, &sockMode);
    
    return 1;
}


int WindSock::ConnectToServer(std::string address, unsigned int port) {
    
    if (isConnected == true) 
        return -2;
    
    // Bind to an IP address and a PORT
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    unsigned long uipAddr = inet_addr(address.c_str());
    hint.sin_addr = (in_addr&)uipAddr;
    
    // Connect to the server
    SOCKET serverSocket = connect(mSocket, (sockaddr*)&hint, sizeof(hint));
    
    if (serverSocket == WSAEWOULDBLOCK) 
        return -1;
    
    isConnected = true;
    return serverSocket;
}

int WindSock::CheckIncomingConnections(void) {
    
    sockaddr_in client;
    int clientSz = sizeof(client);
    
    // Listen for a connection
    SOCKET clientSocket = accept(mSocket, (sockaddr*)&client, &clientSz);
    
    if (clientSocket == WSAEWOULDBLOCK) 
        return -1;
    
    socketList.push_back(clientSocket);
    
    char newHost[NI_MAXHOST];
    char newService[NI_MAXSERV];
    
    ZeroMemory(newHost, NI_MAXHOST);
    ZeroMemory(newService, NI_MAXSERV);
    
    /*int nameInfo = */
    getnameinfo((sockaddr*)&client, clientSz, newHost, NI_MAXHOST, newService, NI_MAXSERV, 0);
    
    host  = newHost;
    //port  = newService;
    hostList.push_back(host);
    
    //std::cout << std::endl;
    //std::cout << host << std::endl;
    //std::cout << newService << std::endl;
    
    return clientSocket;
}

int WindSock::CheckIncomingMessages(char* buffer, unsigned int bufferSize) {
    int numberOfBytes = 0;
    
    // Check incoming messages
    for (unsigned int i=0; i < socketList.size(); i++) {
        SOCKET socket = socketList[i];
        
        numberOfBytes = MessageReceive(socket, buffer, bufferSize);
        if (numberOfBytes == SOCKET_ERROR) 
            continue;
        
        // Client wants to disconnect
        if (buffer[0] == 'X') {
            socketList.erase(socketList.begin() + i);
            hostList.erase(hostList.begin() + i);
            std::cout << "Client disconnected" << std::endl;
            break;
        }
        
    }
    
    if (numberOfBytes == SOCKET_ERROR) 
        numberOfBytes = -1;
    
    return numberOfBytes;
}

int WindSock::DisconnectFromServer(SOCKET socket) {
    char message[] = {'X'}; // Tell the server we are disconnecting
    MessageSend(mSocket, message, sizeof(message));
    isConnected = false;
    return closesocket(socket);
}

void WindSock::MessageSend(SOCKET socket, char* buffer, unsigned int bufferSize) {
    send(socket, buffer, bufferSize, 0);
}

int WindSock::MessageReceive(SOCKET socket, char* buffer, unsigned int bufferSize) {
    return recv(socket, buffer, bufferSize, 0);
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


