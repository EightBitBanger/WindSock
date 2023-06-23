#include <iostream>
#include <fstream>

#define _WIN32_WINNT   0x0601 // Windows 7

#include "CodeBaseLibrary/types.h"
#include "CodeBaseLibrary/strings.h"
#include "CodeBaseLibrary/timer.h"


#include "windsock.h"
#include "socketserver.h"



int main() {
    
    SocketServer serverMain;
    
    if (serverMain.wSock.InitiateServer(80) != 1) {
        std::cout << "Server failed to initiate" << std::endl;
        return 0;
    }
    
    std::cout << "Server ready" << std::endl;
    
    
    while (1) {
        
        //
        // Timeout old connections
        if (serverMain.CheckTimers()) {
            
            std::cout << "Disconnected      " << serverMain.wSock.GetLastAddress().str() << std::endl;
            continue;
        }
        
        //
        // Check new connections
        SOCKET newClient = serverMain.wSock.CheckIncomingConnections();
        
        // Client has connected
        if (newClient != INVALID_SOCKET) {
            std::cout << "Connected         " << serverMain.wSock.GetLastAddress().str() << std::endl;
            std::cout << "active: "<< serverMain.wSock.GetNumberOfSockets() << std::endl;
            continue;
        }
        
        //
        // Check client messages
        int messageSz = serverMain.wSock.CheckIncomingMessages((char*)serverMain.buffer.c_str(), serverMain.buffer.size());
        
        // Client has disconnected
        if (messageSz == 0) {
            std::cout << "Disconnected      " << serverMain.wSock.GetLastAddress().str() << std::endl;
            continue;
        }
        
        //
        // Check client HTTP requests
        
        serverMain.CheckRequest();
        
        
    }
    
    return 1;
}


















