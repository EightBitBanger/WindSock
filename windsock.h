#include <cstdlib>
#include <stdio.h>
#include <algorithm>

#include <iostream>
#include <vector>
#include <string>

#include <iostream>

// Currently set to windows 7 (last good version... ;) )
#define _WIN32_WINNT   0x0601

#include <WS2tcpip.h>


class WindSock {
    
public:
    
    WindSock(void);
    ~WindSock(void);
    
    unsigned int GetLastPort(void)    {return mLastPort;}
    std::string  GetLastHost(void)    {return mLastHost;}
    
    // Messaging
    
    /// Send a message
    void MessageSend(SOCKET socket, char* buffer, unsigned int bufferSize);
    
    /// Receive a message
    int MessageReceive(SOCKET socket, char* buffer, unsigned int bufferSize);
    
    // Client
    
    /// Connect to a server.
    int ConnectToServer(std::string address, unsigned int port);
    
    /// Disconnect from a server.
    int DisconnectFromServer(SOCKET socket);
    
    // Server
    
    /// Start a server listening for incoming connections.
    int InitiateServer(unsigned int portNumber);
    
    /// Check for an incoming connection request.
    int CheckIncomingConnections(void);
    
    /// Check for incoming messages from any connected client.
    int CheckIncomingMessages(char* buffer, unsigned int bufferSize);
    
    // Accessing active server connections
    
    /// Get the number of hosts in the connections list.
    unsigned int GetNumberOfHosts(void) {return mHostList.size();}
    /// Get a host name by its index location in the connections list.
    std::string GetHostIndex(unsigned int index) {return mHostList[index];}
    
    /// Get the number of ports in the connections list.
    unsigned int GetNumberOfPorts(void) {return mPortList.size();}
    /// Get a host name by its index location in the connections list.
    unsigned int GetPortIndex(unsigned int index) {return mPortList[index];}
    
    /// Get the number of hosts in the connections list.
    unsigned int GetNumberOfSockets(void) {return mSocketList.size();}
    /// Get a socket ID by its index location in the connections list.
    SOCKET GetSocketIndex(unsigned int index) {return mSocketList[index];}
    
    
private:
    
    std::string   mLastHost;
    unsigned int  mLastPort;
    
    bool    mIsConnected;
    SOCKET  mSocket;
    
    std::vector<std::string>       mHostList;
    std::vector<unsigned int>      mPortList;
    std::vector<SOCKET>            mSocketList;
    
};


WindSock::WindSock(void) :
    
    mLastHost(""),
    mLastPort(0),
    
    mIsConnected(false),
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

int WindSock::ConnectToServer(std::string address, unsigned int port) {
    
    if (mIsConnected == true) 
        return -2;
    
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    unsigned long uipAddr = inet_addr(address.c_str());
    hint.sin_addr = (in_addr&)uipAddr;
    
    // Connect to the server
    SOCKET serverSocket = connect(mSocket, (sockaddr*)&hint, sizeof(hint));
    
    if (serverSocket == INVALID_SOCKET) 
        return -1;
    
    mIsConnected = true;
    return serverSocket;
}

int WindSock::DisconnectFromServer(SOCKET socket) {
    char message[] = {'X'};
    MessageSend(mSocket, message, sizeof(message));
    mIsConnected = false;
    return closesocket(socket);
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
    
    mIsConnected = true;
    return 1;
}



int WindSock::CheckIncomingConnections(void) {
    
    sockaddr_in client;
    int clientSz = sizeof(client);
    
    // Listen for a connection
    SOCKET clientSocket = accept(mSocket, (sockaddr*)&client, &clientSz);
    
    if (clientSocket == WSAEWOULDBLOCK) 
        return -1;
    
    mSocketList.push_back(clientSocket);
    
    char newHost[NI_MAXHOST];
    char newPort[NI_MAXSERV];
    
    ZeroMemory(newHost, NI_MAXHOST);
    ZeroMemory(newPort, NI_MAXSERV);
    
    /*int nameInfo = */
    getnameinfo((sockaddr*)&client, clientSz, newHost, NI_MAXHOST, newPort, NI_MAXSERV, 0);
    
    mLastHost  = newHost;
    mLastPort  = client.sin_port;
    
    mHostList.push_back(mLastHost);
    mPortList.push_back(mLastPort);
    
    return clientSocket;
}

int WindSock::CheckIncomingMessages(char* buffer, unsigned int bufferSize) {
    int numberOfBytes = 0;
    
    // Check incoming messages
    for (unsigned int i=0; i < mSocketList.size(); i++) {
        SOCKET socket = mSocketList[i];
        
        numberOfBytes = MessageReceive(socket, buffer, bufferSize);
        if (numberOfBytes == SOCKET_ERROR) 
            continue;
        
        // Client wants to disconnect
        if (buffer[0] == 'X') {
            
            // Set the details about the last accessed host
            mLastHost = mHostList[i];
            mLastPort = mPortList[i];
            
            // Remove the client from the server
            mSocketList.erase(mSocketList.begin() + i);
            mHostList.erase(mHostList.begin() + i);
            mPortList.erase(mPortList.begin() + i);
            
            closesocket(socket);
            
            return -2;
        }
        
    }
    
    // No messages
    if (numberOfBytes == SOCKET_ERROR) 
        numberOfBytes = -1;
    
    return numberOfBytes;
}

void WindSock::MessageSend(SOCKET socket, char* buffer, unsigned int bufferSize) {
    send(socket, buffer, bufferSize, 0);
}

int WindSock::MessageReceive(SOCKET socket, char* buffer, unsigned int bufferSize) {
    return recv(socket, buffer, bufferSize, 0);
}

