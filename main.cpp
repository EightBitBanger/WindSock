#include <iostream>
#include <vector>
#include <string>

// Currently set to windows 7 (last good version... ;) )
#define _WIN32_WINNT   0x0601

#include <winsock2.h>
#include <ws2tcpip.h>


int main() {
    
    // Initiate winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    
    int WSOK = WSAStartup(ver, &wsData);
    if (WSOK != 0) {
        std::cout << "Failed to initiate socket" << std::endl;
        return 0;
    }
    
    // Create a socket
    SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == INVALID_SOCKET) {
        std::cout << "Failed to open a socket" << std::endl;
        return 0;
    }
    
    // Bind to an IP address and a PORT
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;
    
    bind(listener, (sockaddr*)&hint, sizeof(hint));
    listen(listener, SOMAXCONN);
    
    // Wait for a connection
    sockaddr_in client;
    int clientSz = sizeof(client);
    
    // Listen for a connection
    
    /*SOCKET clientSocket =*/accept(listener, (sockaddr*)&client, &clientSz);
    
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    
    ZeroMemory(host, NI_MAXHOST);
    ZeroMemory(service, NI_MAXSERV);
    
    if (getnameinfo((sockaddr*)&client, clientSz, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
        std::cout << "Connected" << std::endl;
        std::cout << host << std::endl;
        std::cout << service << std::endl;
    }
    
    return 1;
}

















































/*

#include <cstdlib>
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

int main() {
    
    std::vector<int> arrayOfNumbers;
    arrayOfNumbers.push_back(10);
    arrayOfNumbers.push_back(-10);
    arrayOfNumbers.push_back(8);
    arrayOfNumbers.push_back(39);
    arrayOfNumbers.push_back(18);
    arrayOfNumbers.push_back(130);
    
    int largest=0;
    
    std::sort(arrayOfNumbers.begin(), arrayOfNumbers.end());
    
    for (int i=0; i < 6; i++) {
        if (largest > arrayOfNumbers[i]) 
            largest = arrayOfNumbers[i];
        
        
        std::cout << arrayOfNumbers[i] << " ";
    }
    
    
    
    
    std::cout << std::endl << std::endl;
}

*/
