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
    void RenderHTMLHeader(std::string& buffer, std::string title, std::string color);
    void RenderHTMLFooter(std::string& buffer);
    
    void RenderHTMLDividerLine(std::string& buffer);
    void RenderHTMLNewLine(std::string& buffer);
    
    void RenderHTMLBeginTag(std::string& buffer, std::string tag);
    void RenderHTMLBeginDiv(std::string& buffer);
    void RenderHTMLBeginTagSize(std::string& buffer, std::string tag, unsigned int size);
    void RenderHTMLBeginTagStyle(std::string& buffer, std::string tag, std::string color, unsigned int size);
    
    void RenderHTMLText(std::string& buffer, std::string tag, std::string text, unsigned int size);
    void RenderHTMLLink(std::string& buffer, std::string tag, std::string text, std::string link, std::string color);
    void RenderHTMLImage(std::string& buffer, std::string textLink, unsigned int width, unsigned int height);
    
    void RenderHTMLEndTag(std::string& buffer, std::string tag);
    void RenderHTMLEndDiv(std::string& buffer);
    void RenderHTMLEndTagSize(std::string& buffer, std::string tag, unsigned int size);
    
    
    
    /// Check and handle server requests.
    int CheckRequest(void);
    
    /// Update timers and time out old connections.
    int CheckTimers(void);
    
    
    SocketServer();
    
    
private:
    
    void ProcessGetRequest(unsigned int index, std::string& clientRequest);
    
    void ProcessHeadRequest(unsigned int index, std::string& clientRequest);
    
    void ProcessPostRequest(unsigned int index, std::string& clientRequest);
    
    void ProcessPutRequest(unsigned int index, std::string& clientRequest);
    
    void ProcessDeleteRequest(unsigned int index, std::string& clientRequest);
    
    
    bool ProcessSearchQuery(unsigned int index, std::string& queryString, unsigned int headerBegin, unsigned int headerEnd);
    
};

#endif
