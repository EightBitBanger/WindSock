#include "SocketServer.h"


bool CheckDirectoryExists(std::string directoryName) {
    DWORD dwAttrib = GetFileAttributes( (LPCTSTR)directoryName.data() );
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int CheckFileExists(std::string filename) {
    std::ifstream fStream(filename.c_str(), std::ios::binary | std::ios::ate);
    if (!fStream.is_open()) 
        return -1;
    std::streamsize size = fStream.tellg();
    fStream.close();
    return size;
}

std::vector<std::string> GetFileList(std::string path) {
    std::vector<std::string> results;
    
    HANDLE handle;
    WIN32_FIND_DATA info;
    handle = FindFirstFile((char*)path.c_str(), &info);
    
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


int FileGetSize(std::string filename) {
    std::ifstream fStream(filename.c_str(), std::ios::binary | std::ios::ate);
    if (!fStream.is_open()) 
        return -1;
    
    std::streamsize size = fStream.tellg();
    
    fStream.seekg(0, std::ios::beg);
    
    fStream.close();
    return size;
}



int FileLoadRaw(std::string filename, char* buffer, unsigned int bufferSize) {
    std::ifstream fStream(filename.c_str(), std::ios::binary | std::ios::ate);
    if (!fStream.is_open()) 
        return -1;
    
    std::streamsize size = fStream.tellg();
    
    fStream.seekg(0, std::ios::beg);
    
    if (!fStream.read(buffer, size)) 
        return -1;
    
    fStream.close();
    return size;
}









SocketServer::SocketServer() {
    
    mBuffer.reserve(10000000);
    mBuffer.resize(10000000);
    
}

int SocketServer::CheckRequest(void) {
    
    for (unsigned int i=0; i < wSock.GetNumberOfSockets(); i++) {
        std::string clientRequest = wSock.GetBufferString(i);
        
        if (clientRequest == "") 
            continue;
        
        
        unsigned int end   = clientRequest.find("[break]");
        if (end == std::string::npos) 
            continue;
        
        unsigned int begin = clientRequest.find("GET /");
        int requestType = 0;
        
        if (begin == std::string::npos) {
            begin = clientRequest.find("HEAD /");
            requestType = 1;
        }
        
        if (begin == std::string::npos) {
            begin = clientRequest.find("POST /");
            requestType = 2;
        }
        
        if (begin == std::string::npos) {
            begin = clientRequest.find("PUT /");
            requestType = 3;
        }
        
        if (begin == std::string::npos) {
            begin = clientRequest.find("DELETE /");
            requestType = 4;
        }
        
        if (begin == std::string::npos) 
            continue;
        
        std::string firstRequest;
        firstRequest.resize(1024);
        clientRequest.copy((char*)firstRequest.c_str(), end, begin);
        firstRequest.resize(end - begin);
        
        switch (requestType) {
            
            case 0: ProcessGetRequest(i, firstRequest); break;
            case 1: ProcessHeadRequest(i, firstRequest); break;
            case 2: ProcessPostRequest(i, firstRequest); break;
            case 3: ProcessPutRequest(i, firstRequest); break;
            case 4: ProcessDeleteRequest(i, firstRequest); break;
            
            default:
                break;
            
        }
        
        
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
    buffer += "<br></br>";
}

void SocketServer::RenderHTMLHeader(std::string& buffer, std::string title, std::string color) {
    buffer += "<html>\r\n";
    buffer += "<head><title>" + title + "</title></head>\r\n";
    buffer += "<body bgcolor=" + color + ">\r\n";
}

void SocketServer::RenderHTMLBeginDiv(std::string& buffer) {
    buffer += "<div>";
}

void SocketServer::RenderHTMLBeginTagSize(std::string& buffer, std::string tag, unsigned int size) {
    std::string textSz = IntToString(size);
    buffer += "<" + tag + textSz + ">\r\n";
}

void SocketServer::RenderHTMLBeginTagStyle(std::string& buffer, std::string tag, std::string color, unsigned int size) {
    std::string textSz = IntToString(size);
    buffer += "<" + tag + textSz + " style=\"color:" + color + "\">\r\n";
}

void SocketServer::RenderHTMLBeginTag(std::string& buffer, std::string tag) {
    buffer += "<" + tag + ">\r\n";
}

void SocketServer::RenderHTMLText(std::string& buffer, std::string tag, std::string text, unsigned int size) {
    std::string textSz = IntToString(size);
    buffer += "  <" + tag + textSz + ">" + text + "</" + tag + textSz + ">\r\n";
}

void SocketServer::RenderHTMLLink(std::string& buffer, std::string tag, std::string text, std::string link, std::string color) {
    buffer += "<" + tag + " href=\"" + link + " \" style=\"color:" + color + "\"> " + text + " </" + tag + ">";
}

void SocketServer::RenderHTMLImage(std::string& buffer, std::string textLink, unsigned int width, unsigned int height) {
    std::string widthStr = IntToString(width);
    std::string heightStr = IntToString(height);
    buffer += "  <img src=\"" + textLink + "\" alt=\"img\" width = \"" + widthStr + "\" height = \"" + heightStr + "\">\r\n";
}

void SocketServer::RenderHTMLEndDiv(std::string& buffer) {
    buffer += "</div>";
}

void SocketServer::RenderHTMLEndTag(std::string& buffer, std::string tag) {
    buffer += "</" + tag + ">\r\n";
}

void SocketServer::RenderHTMLEndTagSize(std::string& buffer, std::string tag, unsigned int size) {
    std::string textSz = IntToString(size);
    buffer += "</" + tag + textSz + ">\r\n";
}

void SocketServer::RenderHTMLFooter(std::string& buffer) {
    buffer += "\r\n</body>\r\n";
    buffer += "</html>\r\n";
}













bool SocketServer::ProcessSearchQuery(unsigned int index, std::string& queryString, unsigned int headerBegin, unsigned int headerEnd) {
    
    if (queryString[5] != '?') 
        return false;
    
    // Check this is a search request
    unsigned int begin = queryString.find("/?search=");
    
    if (begin == std::string::npos) 
        return false;
    
    int beginPart = begin+9;
    int endPart   = headerEnd;
    
    std::string query;
    int querySize = endPart - beginPart;
    
    // Isolate the query part of the string
    query.resize(querySize);
    queryString.copy((char*)query.c_str(), endPart, beginPart);
    
    std::transform(query.begin(), query.end(), query.begin(),[](unsigned char c){ return std::tolower(c); });
    
    std::cout << "HTTP query        " << wSock.GetLastAddress().str() << " 200 /?search=" << query << std::endl;
    
    query = StringReplaceAll(query, "+", " ");
    query = StringReplaceAll(query, "_", " ");
    
    // Generate a page of results
    std::string resultHeader = " results for \"" +query+ "\"";
    
    
    std::vector<std::string> fileList;
    
    // Minimal query length
    if (querySize == 0) {
        resultHeader = "Too few characters";
        
    } else {
        // Get files on server
        fileList = GetFileList("*.*");
    }
    
    
    std::vector<std::string> terms = StringExplode(query, ' ');
    std::vector<std::string> matched;
    
    // Get sub directory files
    // Find search terms
    for (unsigned int i=0; i < fileList.size(); i++) {
        std::string filename = fileList[i];
        
        if (CheckDirectoryExists(filename)) {
            
            std::vector<std::string> subFileList = GetFileList(filename + "\\*.*");
            for (unsigned int i=0; i < subFileList.size(); i++) {
                std::string subFilename = subFileList[i];
                fileList.push_back(filename + "\\" + subFilename);
            }
        }
    }
    
    // Find search terms
    for (unsigned int i=0; i < fileList.size(); i++) {
        
        std::transform(fileList[i].begin(), fileList[i].end(), fileList[i].begin(),[](unsigned char c){ return std::tolower(c); });
        
        for (unsigned int t=0; t < terms.size(); t++) {
            if (fileList[i].find(terms[t]) != std::string::npos) {
                matched.push_back(fileList[i]);
                break;
            }
        }
        
        // No terms, accept all
        if (terms.size() == 0) {
            matched = fileList;
        }
        
    }
    
    // File count
    if (querySize > 0) 
        resultHeader = IntToString(matched.size()) + resultHeader;
    
    
    //
    // Render the results page
    
    std::string dataBody;
    RenderHTMLHeader(dataBody, "Search results", "#040418");
    
    // Result header
    RenderHTMLBeginTagStyle(dataBody, "h", "#ffffff", 2);
    RenderHTMLBeginTag(dataBody, "center");
    
    // Header text
    RenderHTMLText(dataBody, "h", resultHeader, 3);
    RenderHTMLEndTagSize(dataBody, "h", 2);
    RenderHTMLDividerLine(dataBody);
    
    // Render the results page
    RenderHTMLBeginTagStyle(dataBody, "h", "#ffffff", 3);
    
    // Results section
    for (unsigned int i=0; i < matched.size(); i++) {
        
        std::string ext = StringGetExtFromFilename(matched[i]);
        
        RenderHTMLBeginTag(dataBody, "left");
        
        RenderHTMLLink(dataBody, "a", matched[i], matched[i], "#2020ff");
        
        RenderHTMLEndTag(dataBody, "left");
        RenderHTMLNewLine(dataBody);
        
    }
    
    // Result footer
    RenderHTMLEndTag(dataBody, "center");
    RenderHTMLEndTagSize(dataBody, "h", 3);
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













void SocketServer::ProcessGetRequest(unsigned int index, std::string& clientRequest) {
    std::string statusCode="200";
    
    // Get requested resource name
    std::string resourceName;
    unsigned int headerBegin = clientRequest.find("GET /");
    unsigned int headerEnd   = clientRequest.find("HTTP/1.1");
    
    if ((headerEnd == std::string::npos) | (headerBegin == std::string::npos)) 
        return;
    
    // Get the web address from the request string
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
        
        resourceName[charPos+1] = '%';
        resourceName[charPos+2] = '%';
        
        charPos = resourceName.find("%%%");
        resourceName = StringReplaceAll(resourceName, "%%%", " ");
        
        unsigned int integer=0;
        std::stringstream ss;
        ss << std::hex << hex;
        ss >> integer;
        resourceName[charPos] = integer;
        
        charPos = resourceName.find('%');
    }
    
    
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
    
    // Load the file contents
    fileSize = FileGetSize(resourceName);
    
    
    if (fileSize > 0) {
        dataBody.reserve(fileSize + 1);
        dataBody.resize(fileSize + 1);
        fileSize = FileLoadRaw(resourceName, (char*)dataBody.data(), dataBody.size());
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
        RenderHTMLHeader(page, "404 Not found", "#ffffff");
        
        RenderHTMLBeginTagStyle(page, "h", "#ffffff", 1);
        RenderHTMLBeginTag(page, "right");
        RenderHTMLText(page, "a", "404", 1);
        RenderHTMLEndTag(page, "right");
        RenderHTMLEndTagSize(page, "h", 1);
        
        RenderHTMLBeginTagStyle(page, "h", "#ffffff", 4);
        RenderHTMLBeginTag(page, "center");
        RenderHTMLText(page, "a", "Not found", 1);
        RenderHTMLEndTag(page, "center");
        RenderHTMLEndTagSize(page, "h", 4);
        
        RenderHTMLFooter(page);
        dataBody = page;
        fileSize = dataBody.size();
    }
    
    std::string bodySzStr = IntToString(fileSize);
    std::string headerLine="";
    
    // Generate an appropriate status line
    
    if (fileType == "css") headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::text_css, CONNECTION::keep_alive);
    if (fileType == "js")  headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::text_js, CONNECTION::keep_alive);
    if ((fileType == "html") | 
        (fileType == "htm")) 
        headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::text_html, CONNECTION::keep_alive);
    
    if ((fileType == "jpg") | (fileType == "jpeg"))
        headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::image_jpeg, CONNECTION::keep_alive);
    if ((fileType == "png") | 
        (fileType == "ico")) 
        headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::image_png, CONNECTION::keep_alive);
    
    // All else fails, download as a file
    if (headerLine == "") 
        headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::application_octet_stream, CONNECTION::keep_alive);
    
    std::cout << "HTTP request      " << wSock.GetLastAddress().str() << " " << statusCode << " /" << resourceName << std::endl;
    
    // Send back the status and requested resource data
    std::string status = headerLine + "\r\n" + dataBody;
    wSock.MessageSend(wSock.GetSocketIndex(index), (char*)status.c_str(), status.size());
    
}

void SocketServer::ProcessHeadRequest(unsigned int index, std::string& clientRequest, std::string& resourceName, unsigned int headerBegin, unsigned int headerEnd) {
    
    std::cout << clientRequest << std::endl << std::endl;
    
}

void SocketServer::ProcessPostRequest(unsigned int index, std::string& clientRequest, std::string& resourceName, unsigned int headerBegin, unsigned int headerEnd) {
    
    std::cout << clientRequest << std::endl << std::endl;
    
}

void SocketServer::ProcessPutRequest(unsigned int index, std::string& clientRequest, std::string& resourceName, unsigned int headerBegin, unsigned int headerEnd) {
    
    std::cout << clientRequest << std::endl << std::endl;
    
}

void SocketServer::ProcessDeleteRequest(unsigned int index, std::string& clientRequest, std::string& resourceName, unsigned int headerBegin, unsigned int headerEnd) {
    
    std::cout << clientRequest << std::endl << std::endl;
    
}



