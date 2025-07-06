#ifndef _WINDSOCK_
#define _WINDSOCK_

#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

#include <vector>
#include <string>
#include <fstream>

#include <stdio.h>
#include <winsock2.h>

#include "Logger.h"

std::string get_mime_type(std::string& filename);
bool fetch_file(const std::string& filename, std::vector<char>& buffer);

std::vector<std::string> split_words(const std::string& input_string);
std::vector<std::string> split_lines(const std::string& input_string);


class ClientRequest {
public:
    bool isReadyToSend;
    
    ClientRequest();
    
    SOCKET socket;
    std::string host;
    std::string port;
    std::string ipaddress;
    
    std::string type;
    std::string request;
    
    /// Data to be returned to the client
    std::string mime;
    std::string buffer;
};

class Windsock {
    
public:
    
    /// Initiate the server.
    int initiate(void);
    
    /// Begin listening on port 80 for HTTP requests.
    int start(void);
    
    /// Check for incoming connection requests.
    int checkRequests(void);
    
    /// Check for incoming connection requests.
    int processRequests(void);
    
    /// Get a client request by its index.
    ClientRequest* getClientRequest(unsigned int index);
    
    /// Remove a client request by its index.
    int removeClientRequest(unsigned int index);
    
    /// Get the number of client requests in queue.
    unsigned int getNumberOfClientRequests(void);
    
    /// Shutdown the server
    void Shutdown(void);
    
    Windsock();
    
    std::string GenerateStatusLine(std::string statusCode, std::string contentLength, std::string contentType, std::string requestedConnectionState);
    
    std::string pageIndex;
    
private:
    
    WSADATA wsaData;
    SOCKET serverSocket;
    SOCKET clientSocket;
    
    struct sockaddr_in server;
    struct sockaddr_in client;
    
    Logger Log;
    
    // Active requests list
    std::vector<ClientRequest*> mClients;
    
};

#endif
