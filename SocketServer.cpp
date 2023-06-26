#include "SocketServer.h"


bool CheckDirectoryExists(std::string directoryName) {
    DWORD dwAttrib = GetFileAttributes( (LPCTSTR)directoryName.data() );
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int CheckFileExists(std::string filename) {
    std::ifstream fStream(filename.c_str(), std::ios::binary | std::ios::ate);
    if (fStream.is_open()) {
        std::streamsize size = fStream.tellg();
        fStream.close();
        return size;
    }
    return -1;
}

std::vector<std::string> GetFileList(std::string path) {
    std::vector<std::string> results;
    
    GetCurrentDirectory(MAX_PATH, (char*)path.c_str());
    
    HANDLE handle;
    WIN32_FIND_DATA info;
    handle = FindFirstFile("*.*", &info);
    
    if (handle == INVALID_HANDLE_VALUE) 
        return results;
    
    while (FindNextFile(handle, &info)) {
        std::string filename = info.cFileName;
        if (filename == "..")
            continue;
        
        results.push_back(filename);
    }
    
    FindClose(handle);
    return results;
}

int FileLoadText(std::string filename, std::string& buffer) {
    std::fstream fStream(filename);
    if (!fStream.is_open()) 
        return -1;
    
    std::string tempBuffer;
    
    while ( getline(fStream, tempBuffer) ) 
        if (tempBuffer != "")
            buffer += tempBuffer;
    
    fStream.close();
    return buffer.size();
}

int FileLoadRaw(std::string filename, char* buffer, unsigned int bufferSize) {
    std::ifstream fStream(filename.c_str(), std::ios::binary | std::ios::ate);
    if (!fStream.is_open()) 
        return -1;
    
    std::streamsize size = fStream.tellg();
    
    fStream.seekg(0, std::ios::beg);
    
    if (!fStream.read(buffer, size)) 
        return -1;
    
    return size;
}









SocketServer::SocketServer() {
    
    mBuffer.reserve(100000000);
    mBuffer.resize(100000000);
    
}

int SocketServer::CheckRequest(void) {
    
    for (unsigned int i=0; i < wSock.GetNumberOfSockets(); i++) {
        std::string clientRequest = wSock.GetBufferString(i);
        
        if (clientRequest == "") 
            continue;
        
        unsigned int begin = clientRequest.find("GET /");
        unsigned int end   = clientRequest.find("[end]");
        
        if ((begin == std::string::npos) | (end == std::string::npos)) 
            continue;
        
        std::string firstRequest;
        firstRequest.resize(1024);
        clientRequest.copy((char*)firstRequest.c_str(), end, begin);
        firstRequest.resize(end - begin);
        
        ProcessGetRequest(i, firstRequest);
        
        wSock.ClearBufferString(i);
        
    }
    
    return 1;
}

std::string SocketServer::GenerateHTTPStatusLine(std::string statusCode, std::string contentLength, std::string contentType, std::string requestedConnectionState) {
    std::string headerLine;
    headerLine  = "HTTP/1.1 "+statusCode+"\r\n";
    headerLine += "Server: WindSock/0.9.0\r\n";
    headerLine += "Date: Mon, 02 Jan 2012 02:33:17 GMT\r\n";
    headerLine += "Content-Type: "+contentType+"\r\n";
    headerLine += "Content-Length: "+contentLength+"\r\n";
    headerLine += "Connection: "+requestedConnectionState+"\r\n";
    return headerLine;
}

void SocketServer::RenderHTMLDividerLine(std::string& buffer) {
    buffer += "<hr>";
}

void SocketServer::RenderHTMLNewLine(std::string& buffer) {
    buffer += "<br>";
}

void SocketServer::RenderHTMLHeader(std::string& buffer, std::string title, std::string colorBackground) {
    buffer += "<html>\r\n";
    buffer += "<head><title>" + title + "</title></head>\r\n";
    buffer += "<body bgcolor=\"" + colorBackground + "\">\r\n";
}

void SocketServer::RenderHTMLBeginDiv(std::string& buffer) {
    buffer += "<div>";
}

void SocketServer::RenderHTMLBeginHeadingBlock(std::string& buffer, unsigned int size) {
    std::string textSz = IntToString(size);
    buffer += "<h" + textSz + ">\r\n";
}

void SocketServer::RenderHTMLBeginHeadingBlockStyle(std::string& buffer, std::string style, unsigned int size) {
    std::string textSz = IntToString(size);
    buffer += "<h" + textSz + " style=\"color:" + style + ";\">\r\n";
}

void SocketServer::RenderHTMLBeginStyle(std::string& buffer, std::string style) {
    buffer += "<" + style + ">\r\n";
}

void SocketServer::RenderHTMLEndStyle(std::string& buffer, std::string style) {
    buffer += "</" + style + ">\r\n";
}

void SocketServer::RenderHTMLText(std::string& buffer, std::string text, unsigned int size) {
    std::string textSz = IntToString(size);
    buffer += "  <h" + textSz + ">" + text + "</h" + textSz + ">\r\n";
}

void SocketServer::RenderHTMLLink(std::string& buffer, std::string text, std::string link, std::string color) {
    buffer += "<a href=\"" + link + " \" style=\"color:" + color + ";\"> " + text + " </a>";
}

void SocketServer::RenderHTMLImage(std::string& buffer, std::string textLink) {
    buffer += "  <img src=\"" + textLink + "\" alt=\"img\">\r\n";
}

void SocketServer::RenderHTMLEndDiv(std::string& buffer) {
    buffer += "</div>";
}

void SocketServer::RenderHTMLEndHeadingBlock(std::string& buffer, unsigned int size) {
    std::string textSz = IntToString(size);
    buffer += "</h" + textSz + ">\r\n";
}

void SocketServer::RenderHTMLFooter(std::string& buffer) {
    buffer += "\r\n</body>\r\n";
    buffer += "</html>\r\n";
}






void SocketServer::ProcessGetRequest(unsigned int index, std::string& clientRequest) {
    std::string statusCode="200";
    
    // Get requested resource name
    std::string resourceName;
    unsigned int headerBegin = clientRequest.find("GET /");
    unsigned int headerEnd   = clientRequest.find("HTTP/1.1");
    
    if ((headerEnd == std::string::npos) | (headerBegin == std::string::npos)) 
        return;
    
    headerBegin +=5;
    headerEnd   -=1;
    
    resourceName.resize(1024);
    clientRequest.copy((char*)resourceName.c_str(), headerEnd, headerBegin);
    resourceName.resize(headerEnd - headerBegin);
    
    // Strip out special symbols
    unsigned int charPos = resourceName.find('%');
    
    while (charPos != std::string::npos) {
        std::string hex = "  ";
        hex[0] = resourceName[charPos+1];
        hex[1] = resourceName[charPos+2];
        unsigned int integer = StringToInt(hex);
        
        resourceName[charPos+1] = '%';
        resourceName[charPos+2] = '%';
        
        charPos = resourceName.find("%%%");
        resourceName = StringReplaceAll(resourceName, "%%%", " ");
        
        std::stringstream ss;
        ss << std::hex << hex;
        ss >> integer;
        resourceName[charPos] = integer;
        
        charPos = resourceName.find('%');
    }
    
    //resourceName = StringReplaceAll(resourceName, "%20", " ");
    
    
    
    //
    // Check is this a search query
    if (ProcessSearchQuery(index, clientRequest, headerBegin, headerEnd)) 
        return;
    
    // Default index request
    if ((resourceName[resourceName.size()-1] == '/') | (resourceName == "")) 
        resourceName += "index.html";
    
    std::string dataBody;
    int fileSize = -1;
    
    // Determine file type
    std::string fileType = StringGetExtFromFilename(resourceName);
    
    // Load text files
    if ((fileType == "html") | 
        (fileType == "htm") | 
        (fileType == "css") | 
        (fileType == "js")) {
        std::string newDataBody;
        fileSize = FileLoadText(resourceName, newDataBody);
        dataBody = newDataBody;
    } else {
        
        // Preallocate buffer
        dataBody.resize(100000000);
        
        fileSize = FileLoadRaw(resourceName, (char*)dataBody.data(), dataBody.size());
        if (fileSize > 0) 
            dataBody.resize(fileSize);
    }
    
    
    // No file exists, run it as a sub directory
    // containing perhaps an index.html?
    if ((fileSize == -1) & (fileType == "")) {
        resourceName += "/index.html";
        fileType = "html";
        std::string newDataBody;
        fileSize = FileLoadText(resourceName, newDataBody);
        dataBody = newDataBody;
    }
    
    // Requested resource does not exist - Throw a 404
    if (fileSize == -1) {
        std::string newDataBody;
        fileSize = FileLoadText("404.html", newDataBody);
        dataBody = newDataBody;
        statusCode = "404";
    }
    
    // No 404 page, generate a default
    if (fileSize == -1) {
        std::string page;
        RenderHTMLHeader(page, "404 Not found", "black");
        
        RenderHTMLBeginHeadingBlockStyle(page, "white", 1);
        RenderHTMLBeginStyle(page, "right");
        RenderHTMLText(page, "404", 1);
        RenderHTMLEndStyle(page, "right");
        RenderHTMLEndHeadingBlock(page, 1);
        
        RenderHTMLBeginHeadingBlockStyle(page, "white", 4);
        RenderHTMLBeginStyle(page, "center");
        RenderHTMLText(page, "Not found", 1);
        RenderHTMLEndStyle(page, "center");
        RenderHTMLEndHeadingBlock(page, 4);
        
        RenderHTMLFooter(page);
        dataBody = page;
        fileSize = dataBody.size();
    }
    
    std::string bodySzStr = IntToString(fileSize);
    std::string headerLine;
    
    // Text content types
    if (fileType == "css") headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::text_css, CONNECTION::keep_alive);
    if (fileType == "js")  headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::text_js, CONNECTION::keep_alive);
    if ((fileType == "html") | 
        (fileType == "htm")) 
        headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::text_html, CONNECTION::keep_alive);
    
    // Image content types
    if ((fileType == "jpg") | (fileType == "jpeg"))
        headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::image_jpeg, CONNECTION::keep_alive);
    if ((fileType == "png") | 
        (fileType == "ico")) 
        headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::image_png, CONNECTION::keep_alive);
    
    // Video content types
    //if ((fileType == "mp4")) 
    //    headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::application_octet_stream, CONNECTION::keep_alive);
    
    // All else fails, download the file
    if (headerLine == "") 
        headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::application_octet_stream, CONNECTION::keep_alive);
    
    std::cout << "HTTP request      " << wSock.GetLastAddress().str() << " " << statusCode << " /" << resourceName << std::endl;
    
    // Send back the status and requested resource data
    std::string status = headerLine + "\r\n" + dataBody;
    wSock.MessageSend(wSock.GetSocketIndex(index), (char*)status.c_str(), status.size());
    
}






bool SocketServer::ProcessSearchQuery(unsigned int index, std::string& queryString, unsigned int headerBegin, unsigned int headerEnd) {
    
    if (queryString[5] != '?') 
        return false;
    
    // Check this is a search request
    unsigned int begin = queryString.find("/?search=");
    
    if (begin == std::string::npos) 
        return false;
    
    unsigned int beginPart = begin+9;
    unsigned int endPart   = headerEnd;
    
    std::string query;
    
    query.resize(1024);
    queryString.copy((char*)query.c_str(), endPart, beginPart);
    query.resize(endPart - beginPart);
    
    query = StringReplaceAll(query, "+", " ");
    
    std::cout << "HTTP query        " << wSock.GetLastAddress().str() << " 200 /?search=" << query << std::endl;
    
    // Generate a page of results
    std::string dataBody;
    std::string resultHeader = "Results \"" +query+ "\"";
    RenderHTMLHeader(dataBody, "Search results", "black");
    
    // Result header
    RenderHTMLBeginHeadingBlockStyle(dataBody, "white", 2);
    RenderHTMLBeginStyle(dataBody, "left");
    RenderHTMLText(dataBody, resultHeader, 3);
    RenderHTMLEndHeadingBlock(dataBody, 2);
    
    
    // Render the results page
    RenderHTMLBeginHeadingBlockStyle(dataBody, "white", 3);
    
    std::vector<std::string> results;
    results = GetFileList(query);
    
    for (unsigned int i=0; i < results.size(); i++) {
        
        if (results[i].find(query) == std::string::npos) 
            continue;
        
        RenderHTMLDividerLine(dataBody);
        
        RenderHTMLBeginStyle(dataBody, "left");
        RenderHTMLLink(dataBody, results[i], results[i], "blue");
        RenderHTMLEndStyle(dataBody, "left");
        
    }
    
    RenderHTMLEndStyle(dataBody, "left");
    
    RenderHTMLEndHeadingBlock(dataBody, 3);
    
    RenderHTMLFooter(dataBody);
    
    // Disable any other search requests
    unsigned int pos = queryString.find("/?search=");
    while (pos != std::string::npos) {
        queryString[pos+1] = ' ';
        pos = queryString.find("/?search=");
    }
    
    std::string headerLine = GenerateHTTPStatusLine(STATECODE::ok, IntToString(dataBody.size()), CONTENTTYPE::text_html, CONNECTION::keep_alive);
    
    std::string status = headerLine + "\r\n" + dataBody;
    wSock.MessageSend(wSock.GetSocketIndex(index), (char*)status.c_str(), status.size());
    return true;
}





void SocketServer::ProcessPutRequest(unsigned int index, std::string& clientRequest, std::string& resourceName, unsigned int headerBegin, unsigned int headerEnd) {
    
    std::cout << clientRequest << std::endl << std::endl;
    
}

void SocketServer::ProcessDeleteRequest(unsigned int index, std::string& clientRequest, std::string& resourceName, unsigned int headerBegin, unsigned int headerEnd) {
    
    std::cout << clientRequest << std::endl << std::endl;
    
}



