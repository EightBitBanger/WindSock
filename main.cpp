#include <iostream>
#include <fstream>

#define _WIN32_WINNT   0x0601 // Windows 7

#include "windsock.h"
#define _RUN_SERVER_



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
        buffer += "\r\n";
    }
    
    fStream.close();
    return buffer.size();
}

std::string GenerateHTTPStatusLine(std::string statusCode, std::string contentLength, std::string contentType, std::string requestedConnectionState) {
    std::string headerLine;
    headerLine  = "HTTP/1.1 "+statusCode+"\r\n";
    headerLine += "Server: nginx/0.8.54\r\n";
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






void RunServer(void) {
    
    // Server console messages
    std::string msgConnect   = "Connected         ";
    std::string msgDisConn   = "Disconnected      ";
    std::string msgGetRqst   = "HTTP request      ";
    
    WindSock wSock;
    
    wSock.InitiateServer(80);
    
    std::cout << "Server ready" << std::endl;
    
    std::string buffer;
    buffer.reserve(32768);
    buffer.resize(8096);
    
    while (1) {
        
        //
        // Check new connections
        SOCKET newClient = wSock.CheckIncomingConnections();
        
        // No new connections
        if (newClient != INVALID_SOCKET) {
            std::cout << msgConnect << wSock.GetLastAddress().str() << std::endl;
            continue;
        }
        
        //
        // Check client messages
        int messageSz = wSock.CheckIncomingMessages((char*)buffer.c_str(), buffer.size());
        
        // Client has disconnected
        if (messageSz == 0) {
            std::cout << msgDisConn << wSock.GetLastAddress().str() << std::endl;
            continue;
        }
        
        //
        // Check socket messages
        
        for (unsigned int i=0; i < wSock.GetNumberOfSockets(); i++) {
            std::string clientRequest = wSock.GetBufferString(i);
            
            if (clientRequest == "") 
                continue;
            
            // Check client HTTP get request
            if (clientRequest.compare(0, 5, "GET /") == 0) {
                if (clientRequest.compare(clientRequest.size()-4, 4, "\r\n\r\n") == 0) {
                    
                    // Find GET requests
                    for (unsigned int a=0; a < 32; a++) {
                        unsigned int headerBegin = clientRequest.find("GET /");
                        unsigned int headerEnd   = clientRequest.find("HTTP/1.1")-1;
                        
                        if (headerBegin == std::string::npos) 
                            break;
                        
                        // Get requested resource name
                        std::string resourceRequest(1024, ' ');
                        resourceRequest.resize( headerEnd - headerBegin - 5);
                        
                        clientRequest.copy((char*)resourceRequest.c_str(), headerEnd - 5, headerBegin + 5);
                        resourceRequest.shrink_to_fit();
                        
                        // Kill the request as it has been processed
                        clientRequest[headerBegin+1] = ' ';
                        
                        // Default html request
                        if (resourceRequest == "") 
                            resourceRequest = "index.html";
                        
                        // Server up the resource
                        std::cout << msgGetRqst << wSock.GetLastAddress().str() << " /" << resourceRequest << std::endl;
                        
                        // Load the resource from file
                        std::string dataBody;
                        int fileSize = LoadFileHTML(resourceRequest, dataBody);
                        
                        // Requested file does not exist
                        if (fileSize == -1) 
                            fileSize = LoadFileHTML("404.html", dataBody);
                        if (fileSize == -1) 
                            break;
                        
                        // Generate an HTML status
                        std::stringstream sStream;
                        sStream << fileSize;
                        std::string bodySzStr = sStream.str();
                        
                        std::string headerLine = GenerateHTTPStatusLine(STATECODE::ok, bodySzStr, CONTENTTYPE::text_html, CONNECTION::keep_alive);
                        
                        // Send back the status and resource data
                        std::string status = headerLine + "\r\n" + dataBody;
                        wSock.MessageSend(wSock.GetSocketIndex(i), (char*)status.c_str(), status.size());
                    }
                    
                    wSock.ClearBufferString(i);
                    
                }
                
                continue;
            }
            
        }
        
    }
    
}



void RunClient() {
    
    //while (1) {
    
    WindSock wSock;
    
    SOCKET server = wSock.ConnectToServer("192.168.200.150", 80);
    
    // Unable to connect
    if (server == INVALID_SOCKET) {
        std::cout << "Cannot connect to the server." << std::endl;
        return;
    }
    
    std::cout << "Connected." << std::endl;
    
    std::string input;
    std::cin >> input;
    
    wSock.DisconnectFromServer(server);
    
    //}
    
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


















