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

#include "windsock.h"
#include "ServerTypes.h"


bool CheckDirectoryExists(std::string directoryName);

int CheckFileExists(std::string filename);

int FileLoadText(std::string filename, std::string& buffer);

int FileLoadRaw(std::string filename, char* buffer, unsigned int bufferSize);

std::vector<std::string> GetFileList(std::string path);






class SocketServer {
    
public:
    
    WindSock wSock;
    
    std::string mBuffer;
    
    // HTTP generation
    
    /// Generate an HTTP status header using the given parameters.
    std::string GenerateHTTPStatusLine(std::string statusCode, std::string contentLength, std::string contentType, std::string requestedConnectionState);
    
    // HTML rendering
    
    void RenderHTMLDividerLine(std::string& buffer);
    void RenderHTMLNewLine(std::string& buffer);
    
    void RenderHTMLHeader(std::string& buffer, std::string title, std::string colorBackground);
    
    void RenderHTMLBeginDiv(std::string& buffer);
    void RenderHTMLBeginHeadingBlock(std::string& buffer, unsigned int size);
    void RenderHTMLBeginHeadingBlockStyle(std::string& buffer, std::string style, unsigned int size);
    
    void RenderHTMLBeginStyle(std::string& buffer, std::string style);
    void RenderHTMLEndStyle(std::string& buffer, std::string style);
    
    void RenderHTMLText(std::string& buffer, std::string text, unsigned int size);
    void RenderHTMLLink(std::string& buffer, std::string text, std::string link, std::string color);
    void RenderHTMLImage(std::string& buffer, std::string textLink);
    
    void RenderHTMLEndDiv(std::string& buffer);
    void RenderHTMLEndHeadingBlock(std::string& buffer, unsigned int size);
    
    void RenderHTMLFooter(std::string& buffer);
    
    
    /// Check and handle server requests.
    int CheckRequest(void);
    
    /// Update timers and time out old connections.
    int CheckTimers(void);
    
    
    SocketServer();
    
    
private:
    
    // Check and process HTTP get requests
    void ProcessGetRequest(unsigned int index, std::string& clientRequest);
    
    // Check and process HTTP put requests
    void ProcessPutRequest(unsigned int index, std::string& clientRequest, std::string& resourceName, unsigned int headerBegin, unsigned int headerEnd);
    
    // Check and process HTTP delete requests
    void ProcessDeleteRequest(unsigned int index, std::string& clientRequest, std::string& resourceName, unsigned int headerBegin, unsigned int headerEnd);
    
    // Process an incoming field
    bool ProcessSearchQuery(unsigned int index, std::string& queryString, unsigned int headerBegin, unsigned int headerEnd);
    
};

#endif
