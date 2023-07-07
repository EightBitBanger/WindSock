#ifndef _SOCKET_SERVER_
#define _SOCKET_SERVER_

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "CodeBaseLibrary/logging.h"
#include "CodeBaseLibrary/types.h"
#include "CodeBaseLibrary/strings.h"
#include "CodeBaseLibrary/timer.h"
#include "CodeBaseLibrary/filesystem.h"

#include "windsock.h"
#include "renderer.h"
#include "ServerTypes.h"




class SocketServer {
    
public:
    
    /// Socket layer
    WindSock wSock;
    
    std::string mBuffer;
    
    /// Generate an HTTP status header using the given parameters.
    std::string GenerateHTTPStatusLine(std::string statusCode, std::string contentLength, std::string contentType, std::string requestedConnectionState);
    
    
    /// Check and handle client HTTP requests.
    int CheckHTTPRequest(void);
    
    /// Update timers and time out old connections.
    int CheckTimers(void);
    
    
    SocketServer();
    
    
private:
    
    void ProcessGetRequest(unsigned int index, std::string& request);
    void ProcessHeadRequest(unsigned int index, std::string& request);
    void ProcessPostRequest(unsigned int index, std::string& request);
    void ProcessPutRequest(unsigned int index, std::string& request);
    void ProcessDeleteRequest(unsigned int index, std::string& request);
    
    bool ProcessSearchQuery(unsigned int index, std::string& queryString);
    
    void SendHTTPResponse();
    
};

#endif
