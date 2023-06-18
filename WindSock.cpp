#include "windsock.h"
#define _RUN_SERVER_




void RunServer(void) {
    
    WindSock wSock;
    
    wSock.InitiateServer(80);
    
    std::cout << "Server ready" << std::endl;
    
    char buffer[1024];
    
    while (1) {
        
        Sleep(10);
        
        int socket = wSock.CheckIncomingMessages(buffer, sizeof(buffer));
        
        // Socket disconnected
        if (socket == 0) {
            std::cout << "Disconnected  " << wSock.GetLastHost() << " : " << wSock.GetLastPort() << std::endl;
            continue;
        }
        
        // Check new connections
        int clientSocket = wSock.CheckIncomingConnections();
        
        // No new connections
        if (clientSocket == -1) 
            continue;
        
        std::cout << "Connected     " << wSock.GetLastHost() << " : " << wSock.GetLastPort() << std::endl;
    }
    
}



void RunClient() {
    
    
    while(1) {
        
        WindSock wSock;
        
        SOCKET server = wSock.ConnectToServer("192.168.200.150", 80);
        
        // Unable to connect
        if (server == INVALID_SOCKET) {
            std::cout << "Cannot connect to the server." << std::endl;
            return;
        }
        
        std::cout << "Connected." << std::endl;
        
        //std::string input;
        //std::cin >> input;
        
        wSock.DisconnectFromServer(server);
    }
    
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


















