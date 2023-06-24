#ifndef _WINDSOCK_
#define _WINDSOCK_

#include <vector>
#include <string>
#include <sstream>

#include <WS2tcpip.h>

#ifndef CONNECTION_TIMEOUT
 #define CONNECTION_TIMEOUT  120   // Seconds
#endif

class IPAddress {
    
public:
    
    unsigned char addr[4] = {0, 0, 0, 0};
    
    std::string str();
    
};
    


class WindSock {
    
public:
    
    WindSock(void);
    ~WindSock(void);
    
    /// Start a server listening for incoming connections.
    int InitiateServer(unsigned int port, unsigned int maxConn=200);
    /// Remove a client from the server.
    int DisconnectFromClient(unsigned int index);
    
    /// Check for an incoming connection request.
    SOCKET CheckIncomingConnections(void);
    /// Check for incoming messages from any connected client.
    int CheckIncomingMessages(char* buffer, unsigned int bufferSize);
    
    /// Port number from the last client to access the server.
    unsigned int GetLastPort(void);
    /// Host name from the last client to access the server.
    std::string  GetLastHost(void);
    /// IP address from the last client to access the server.
    IPAddress    GetLastAddress(void);
    /// Index position of the last client to access the server.
    unsigned int GetLastIndex(void);
    
    // Active connections
    
    /// Get the number of hosts in the connections list.
    unsigned int GetNumberOfHosts(void);
    /// Get a host name by its index location in the connections list.
    std::string GetHostIndex(unsigned int index);
    /// Find a host index location by its name.
    int FindHost(std::string name);
    
    /// Get the number of ports from the connections list.
    unsigned int GetNumberOfPorts(void) {return mPortList.size();}
    /// Get a port by its index location in the connections list.
    unsigned int GetPortIndex(unsigned int index) {return mPortList[index];}
    
    /// Get the number of sockets from the connections list.
    unsigned int GetNumberOfSockets(void) {return mSocketList.size();}
    /// Get a socket by its index location in the connections list.
    SOCKET GetSocketIndex(unsigned int index) {return mSocketList[index];}
    
    /// Get a buffer string from a socket index location in the connections list.
    std::string GetBufferString(unsigned int index) {return mBufferList[index];}
    /// Get a buffer string from a socket index location in the connections list.
    void ClearBufferString(unsigned int index) {mBufferList[index] = "";}
    
    /// Get a timeout number from the connections list.
    int GetNumberOfTimers(void) {return mTimeoutList.size();}
    /// Get a timer by its index location in the connections list.
    int GetTimerIndex(unsigned int index) {return mTimeoutList[index];}
    /// Set a timer value by its index location in the connections list.
    void SetTimerValue(unsigned int index, int value) {mTimeoutList[index] = value;}
    
    
    // Messaging
    
    /// Send a message
    void MessageSend(SOCKET socket, char* buffer, unsigned int bufferSize);
    
    /// Receive a message
    int MessageReceive(SOCKET socket, char* buffer, unsigned int bufferSize);
    
    
private:
    
    std::string   mLastHost;
    unsigned int  mLastPort;
    IPAddress     mLastAddress;
    unsigned int  mLastIndex;
    
    bool    mIsConnected;
    SOCKET  mSocket;
    
    std::vector<std::string>       mHostList;
    std::vector<unsigned int>      mPortList;
    std::vector<IPAddress>         mAddressList;
    std::vector<SOCKET>            mSocketList;
    std::vector<std::string>       mBufferList;
    std::vector<int>               mTimeoutList;
    
};

#endif
