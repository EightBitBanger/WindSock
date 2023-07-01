#include <iostream>
#include <fstream>

#define _WIN32_WINNT   0x0601 // Windows 7

#include "CodeBaseLibrary/logging.h"
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
        
        // Check timeout on old connections
        serverMain.wSock.CheckTimers();
        
        // Check new connections
        serverMain.wSock.CheckIncomingConnections();
        
        //
        // Check client messages
        serverMain.wSock.CheckIncomingMessages((char*)serverMain.mBuffer.c_str(), serverMain.mBuffer.size());
        
        //
        // Check client HTTP requests
        serverMain.CheckRequest();
        
        
    }
    
    return 1;
}


















