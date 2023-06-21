//
// Still under construction... may be chaotic

int LoadFileHTML(std::string filename, std::string& buffer) {
    std::fstream fStream;
    
    fStream.open( filename );
    if (!fStream.is_open()) 
        return -1;
    
    std::string tempBuffer;
    
    // Load the data from file
    while ( getline(fStream, tempBuffer) ) {
        
        if (tempBuffer == "")
            continue;
        
        buffer += tempBuffer;
        //buffer += "\r\n";
    }
    
    
    fStream.close();
    return buffer.size();
}

std::string GenerateHTTPStatusLine(std::string statusCode, std::string contentLength, std::string contentType, std::string requestedConnectionState) {
    std::string headerLine;
    headerLine  = "HTTP/1.1 "+statusCode+"\r\n";
    headerLine += "Server: WindSock/0.9.0\r\n";
    headerLine += "Date: Mon, 02 Jan 2012 02:33:17 GMT\r\n";
    headerLine += "Content-Type: "+contentType+"\r\n";
    headerLine += "Content-Length: "+contentLength+"\r\n";
    headerLine += "Connection: "+requestedConnectionState+"\r\n";
    return headerLine;
}


namespace STATECODE {
    // General
    const char ok[]               = "200 OK";
    const char not_found[]        = "404 Not found";
    const char processing[]       = "102 Processing";
    const char no_content[]       = "204 No Content";
    // Errors
    const char bad_request[]      = "400 Bad Request";
    const char unauthorized[]     = "401 Unauthorized";
    const char forbidden[]        = "403 Forbidden";
    // Internal errors
    const char server_error[]     = "500 Internal Server Error";
    const char bad_gateway[]      = "502 Bad Gateway";
    const char gateway_timeout[]  = "504 Gateway Timeout";
    const char unavailable[]      = "503 Service Unavailable";
}

namespace CONTENTTYPE {
    // Text
    const char text_plain[]    = "text/plain";
    const char text_html[]     = "text/html";
    const char text_xml[]      = "text/xml";
    const char text_rtf[]      = "text/rtf";
    const char text_css[]      = "text/css";
    const char text_csv[]      = "text/csv";
    const char text_csvsch[]   = "text/csv-schema";
    const char text_md[]       = "text/markdown";
    const char text_js[]       = "text/javascript";
    // Images
    const char image_jpeg[]    = "image/jpeg";
    const char image_png[]     = "image/png";
    const char image_bmp[]     = "image/bmp";
    const char image_svgxml[]  = "image/svg+xml";
    // Video
    const char video_mp4[]     = "video/mp4";
    const char video_ogg[]     = "video/ogg";
    // Audio
    const char audio_ogg[]     = "audio/ogg";
    // Model
    const char model_obj[]     = "model/obj";
    const char model_mtl[]     = "model/mtl";
    // Application
    const char application_octet_stream[]     = "application/octet-stream";
}

namespace CONNECTION {
    // Close connection after initial communication
    const char close[]         = "close";
    // Keep connection open after initial communication
    const char keep_alive[]    = "keep-alive";
}




class SocketServer {
    
public:
    
    std::string buffer;
    
    int CheckRequest(void);
    
    std::string GenerateHTMLpage404(void);
    
    
    SocketServer();
    
    
    
    WindSock wSock;
    
    
    
private:
    
    
    
    
    
    
};

SocketServer::SocketServer() {
    
    buffer.reserve(32768);
    buffer.resize(8096);
    
}

int SocketServer::CheckRequest(void) {
    
    std::string statusCode="200";
    std::string resourceRequest(1024, ' ');
    
    for (unsigned int i=0; i < wSock.GetNumberOfSockets(); i++) {
        std::string clientRequest = wSock.GetBufferString(i);
        
        if (clientRequest == "") 
            continue;
        
        // Check HTTP request
        if (clientRequest.compare(clientRequest.size()-4, 4, "\r\n\r\n") != 0) 
            continue;
        
        // Process HTTP get requests
        for (unsigned int a=0; a < 100; a++) {
            
            unsigned int headerEnd   = clientRequest.find("HTTP/1.1")-1;
            
            // Check bad request
            if (headerEnd == std::string::npos) 
                continue;
            
            // Determine the request type
            // GET  HEAD  POST  PUT  DELETE  TRACE  CONNECT
            
            unsigned int headerBegin = clientRequest.find("GET /");
            
            if (headerBegin == std::string::npos) 
                continue;
            
            // Get requested resource name
            resourceRequest.resize( headerEnd - headerBegin - 5);
            
            clientRequest.copy((char*)resourceRequest.c_str(), headerEnd - 5, headerBegin + 5);
            resourceRequest.shrink_to_fit();
            
            // Kill the old request as it has been processed
            clientRequest[headerBegin+1] = ' ';
            
            // Default index request
            if (resourceRequest == "") 
                resourceRequest = "index.html";
            
            // Load the resource from file
            std::string dataBody;
            int fileSize = LoadFileHTML(resourceRequest, dataBody);
            
            // Requested file does not exist - Throw a 404
            if (fileSize == -1) {
                fileSize = LoadFileHTML("404.html", dataBody);
                statusCode = "404";
            }
            
            // No 404 page, generate a default
            if (fileSize == -1) {
                dataBody = GenerateHTMLpage404();
                fileSize = dataBody.size();
            }
            
            
            std::string bodySzStr = IntToString(fileSize);
            
            std::string headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::text_html, CONNECTION::keep_alive);
            
            
            std::cout << "HTTP request      " << wSock.GetLastAddress().str() << " " << statusCode << " /" << resourceRequest << std::endl;
            
            // Send back the status and resource data
            std::string status = headerLine + "\r\n" + dataBody;
            wSock.MessageSend(wSock.GetSocketIndex(i), (char*)status.c_str(), status.size());
        }
        
        wSock.ClearBufferString(i);
        
    }
    
    return 1;
}

std::string SocketServer::GenerateHTMLpage404(void) {
    std::string page;
    page += "<html>";
    page += "<head><title>404 Not found </title></head>";
    page += "<body bgcolor=\"black\">";
    page += "<h1 style=\"color:gray;\">";
    page += "  <center><h1>  404 </h1></center>";
    page += "  <center><h3> Page not found </h3></center>";
    page += "</h1>";
    page += "</body>";
    page += "</html>";
    return page;
}





