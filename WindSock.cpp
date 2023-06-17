#include "windsock.h"
//#define _RUN_SERVER_




void RunServer(void) {
    
    WindSock wSock;
    
    wSock.InitiateServer(80);
    
    std::cout << "Server ready" << std::endl;
    
    char buffer[1024];
    
    while (1) {
        
        wSock.CheckIncomingMessages(buffer, sizeof(buffer));
        
        // Check new connections
        int clientSocket = wSock.CheckIncomingConnections();
        
        if (clientSocket == -1) 
            continue;
        
        std::cout << "Connected: " << wSock.GetHost() << std::endl;
        
    }
    
}



void RunClient() {
    
    WindSock wSock;
    
    int server = wSock.ConnectToServer("192.168.200.150", 80);
    
    // Unable to connect
    if (server == -1) {
        std::cout << "Cannot connect to the server" << std::endl;
        return;
    }
    
    Sleep(1000);
    
    wSock.DisconnectFromServer(server);
    
}





int main() {
    
#ifdef _RUN_SERVER_
    RunServer();
#endif
    
#ifndef _RUN_SERVER_
    RunClient();
#endif
    
    return 1;
}


















