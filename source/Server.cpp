#include "Server.h"









SocketServer::SocketServer() {
    
    mBuffer.reserve(10000000);
    mBuffer.resize(10000000);
    
}

int SocketServer::CheckHTTPRequest(void) {
    
    for (unsigned int i=0; i < wSock.GetNumberOfSockets(); i++) {
        std::string clientRequest = wSock.GetBufferString(i);
        
        if (clientRequest == "") 
            continue;
        
        unsigned int end   = clientRequest.find("[break]");
        if (end == std::string::npos) 
            continue;
        
        unsigned int begin = clientRequest.find("GET /");
        int requestType = 0;
        
        if (begin == std::string::npos) {begin = clientRequest.find("HEAD /"); requestType = 1;}
        if (begin == std::string::npos) {begin = clientRequest.find("POST /"); requestType = 2;}
        if (begin == std::string::npos) {begin = clientRequest.find("PUT /"); requestType = 3;}
        if (begin == std::string::npos) {begin = clientRequest.find("DELETE /"); requestType = 4;}
        
        if (begin == std::string::npos) 
            continue;
        
        unsigned int requestSz = end - begin;
        
        std::string firstRequest;
        firstRequest.reserve(requestSz);
        firstRequest.resize(requestSz);
        clientRequest.copy((char*)firstRequest.c_str(), requestSz, begin);
        
        switch (requestType) {
            case 0: ProcessGetRequest(i, firstRequest); break;
            case 1: ProcessHeadRequest(i, firstRequest); break;
            case 2: ProcessPostRequest(i, firstRequest); break;
            case 3: ProcessPutRequest(i, firstRequest); break;
            case 4: ProcessDeleteRequest(i, firstRequest); break;
            default: break;
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





bool SocketServer::ProcessSearchQuery(unsigned int index, std::string& queryString) {
    std::string searchString = "?search=";
    
    size_t searchBegin = queryString.find(searchString);
    
    if (searchBegin == std::string::npos) 
        return false;
    
    unsigned int requestSz = queryString.length() - searchString.length();
    
    std::string query;
    query.reserve(requestSz);
    query.resize(requestSz);
    queryString.copy((char*)query.c_str(), requestSz, searchBegin + searchString.length());
    
    // Lower case the string
    std::transform(query.begin(), query.end(), query.begin(),[](unsigned char c){ return std::tolower(c); });
    
    std::cout << "HTTP query        " << wSock.GetLastAddress().str() << " 200 /?search=" << query << std::endl;
    
    query = StringReplaceAll(query, "+", " ");
    query = StringReplaceAll(query, "_", " ");
    
    // Generate a page of results
    std::string resultHeader = " results for \"" +query+ "\"";
    
    std::vector<std::string> fileList;
    
    // Minimal query length
    if (query.length() == 0) {
        resultHeader = "Too few characters";
        
    } else {
        // Get files on server
        fileList = DirectoryGetFileList("*.*");
    }
    
    
    std::vector<std::string> terms = StringExplode(query, ' ');
    std::vector<std::string> matched;
    
    // Get sub directory files
    // Find search terms
    for (unsigned int i=0; i < fileList.size(); i++) {
        std::string filename = fileList[i];
        
        if (DirectoryCheckExists(filename)) {
            
            std::vector<std::string> subFileList = DirectoryGetFileList(filename + "\\*.*");
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
        
        // No search terms, accept all
        if (terms.size() == 0) {
            matched = fileList;
        }
        
    }
    
    // File count
    if (query.length() > 0) 
        resultHeader = IntToString(matched.size()) + resultHeader;
    
    
    //
    // Render the results page
    
    HTMLRenderer page;
    page.header("Search results", "#040418");
    page.beginTagStyle("h", "#ffffff", 2);
    page.beginTag("center");
    
    page.text("h", resultHeader, 3);
    page.endTagSize("h", 2);
    page.dividerLine();
    
    page.beginTagStyle("h", "#ffffff", 3);
    
    // Results section
    for (unsigned int i=0; i < matched.size(); i++) {
        
        std::string ext = StringGetExtFromFilename(matched[i]);
        
        page.beginTag("left");
        
        page.link("a", matched[i], matched[i], "#2020ff");
        
        if ((ext == "jpeg") | 
            (ext == "jpg") | 
            (ext == "ico") | 
            (ext == "png")) {
            
            page.newLine();
            page.image(matched[i], 100, 100);
        }
        
        page.endTag("left");
        page.newLine();
    }
    
    // Result footer
    page.endTag("center");
    
    page.endTagSize("h", 3);
    page.footer();
    
    std::string pageString = page.bufferFlush();
    
    std::string headerLine = GenerateHTTPStatusLine(STATECODE::ok, IntToString(pageString.size()), CONTENTTYPE::text_html, CONNECTION::keep_alive);
    
    std::string status = headerLine + "\r\n" + pageString;
    wSock.MessageSend(wSock.GetSocketIndex(index), (char*)status.c_str(), status.size());
    
    return true;
}






void SocketServer::SendHTTPResponse() {
    
    
    
    
}








void SocketServer::ProcessGetRequest(unsigned int index, std::string& request) {
    std::string requestHeader = "GET /";
    std::string requestEnd    = "HTTP/1.1";
    
    unsigned int headerBegin = request.find(requestHeader);
    unsigned int headerEnd   = request.find(requestEnd);
    
    if ((headerEnd == std::string::npos) | (headerBegin == std::string::npos)) 
        return;
    
    unsigned int rcNameBegin  = headerBegin + requestHeader.length();
    unsigned int rcNameEnd    = headerEnd - 1;
    unsigned int rcNameSz     = rcNameEnd - rcNameBegin;
    
    // Get the web resource name from the request string
    std::string resourceName;
    resourceName.reserve(rcNameSz);
    resourceName.resize(rcNameSz);
    request.copy((char*)resourceName.c_str(), rcNameSz, rcNameBegin);
    
    // Strip out special symbols
    size_t charPos = resourceName.find('%');
    while (charPos != std::string::npos) {
        std::string hex = "  ";
        hex[0] = resourceName[charPos+1];
        hex[1] = resourceName[charPos+2];
        
        resourceName.erase(charPos+1, 2);
        
        unsigned int hexChar;
        std::stringstream ss;
        ss << std::hex << hex;
        ss >> hexChar;
        
        // Unacceptable chars
        if (hexChar == '\\') 
            hexChar = '/';
        
        resourceName[charPos] = hexChar;
        
        charPos = resourceName.find('%', charPos+1);
    }
    
    
    
    
    
    // Check is this a search query
    if (ProcessSearchQuery(index, resourceName)) 
        return;
    
    //
    // Attempt to locate the desired resource file and return it to the client
    SendHTTPresponse();
    
    std::string body = "";
    std::string bodySzStr = IntToString(body.length());
    
    std::string headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, "", CONNECTION::keep_alive);
    
    // Send back the status and requested resource data
    std::string status = headerLine + "\r\n" + body;
    
    wSock.MessageSend(wSock.GetSocketIndex(index), (char*)status.c_str(), status.size());
    
    
    /*
    
    if ((resourceName[resourceName.size()-1] == '/') | (resourceName == "")) 
        resourceName += "index.html";
    
    std::string dataBody;
    int fileSize = -1;
    
    // Determine file type
    std::string fileType = StringGetExtFromFilename(resourceName);
    
    // Load the file contents
    fileSize = FileGetSize(resourceName);
    
    if (fileSize > 0) {
        dataBody.reserve(fileSize);
        dataBody.resize(fileSize);
        fileSize = FileLoadRaw(resourceName, (char*)dataBody.data(), dataBody.size());
    }
    
    // No file exists, run it as a sub directory
    // Try to find an index.html
    if ((fileSize == -1) & (fileType == "")) {
        resourceName += "/index.html";
        std::string newDataBody;
        fileSize = FileLoadText(resourceName, newDataBody);
        fileType = "html";
        dataBody = newDataBody;
    }
    
    std::string stateCode   = STATECODE::ok;
    std::string contentType = "";
    std::string connection  = CONNECTION::keep_alive;
    
    // Requested resource does not exist - Throw a 404
    if (fileSize == -1) {
        std::string newDataBody;
        fileSize = FileLoadText("404.html", newDataBody);
        fileType = "html";
        dataBody = newDataBody;
        stateCode = STATECODE::not_found;
    }
    
    std::string bodySzStr = IntToString(fileSize);
    std::string headerLine="";
    
    
    // Generate an appropriate status line
    
    if (fileType == "css")    contentType = CONTENTTYPE::text_css;
    if (fileType == "js")     contentType = CONTENTTYPE::text_js;
    if ((fileType == "html") | 
        (fileType == "htm"))  contentType = CONTENTTYPE::text_html;
    
    if ((fileType == "jpeg") | 
        (fileType == "jpg"))  contentType = CONTENTTYPE::image_jpeg;
    if ((fileType == "png") | 
        (fileType == "ico"))  contentType = CONTENTTYPE::image_png;
    
    // Last resort, stream it
    if (contentType == "")    contentType = CONTENTTYPE::octet_stream;
    
    headerLine = GenerateHTTPStatusLine(stateCode, bodySzStr, contentType, connection);
    
    std::cout << "HTTP request      " << wSock.GetLastAddress().str() << " " << stateCode[0] << stateCode[1] << stateCode[2] << " /" << resourceName << std::endl;
    
    // Send back the status and requested resource data
    std::string status = headerLine + "\r\n" + dataBody;
    
    wSock.MessageSend(wSock.GetSocketIndex(index), (char*)status.c_str(), status.size());
    */
    return;
}


void SocketServer::ProcessHeadRequest(unsigned int index, std::string& request) {
    
    std::cout << request << std::endl << std::endl;
    
}


void SocketServer::ProcessPostRequest(unsigned int index, std::string& request) {
    
    std::cout << request << std::endl << std::endl;
    
}


void SocketServer::ProcessPutRequest(unsigned int index, std::string& request) {
    
    std::cout << request << std::endl << std::endl;
    
}


void SocketServer::ProcessDeleteRequest(unsigned int index, std::string& request) {
    
    std::cout << request << std::endl << std::endl;
    
}



