#include "windsock.h"
#include "renderer.h"

#include <string_view>

#include <WS2tcpip.h>

std::string IntToString(int value);

bool get_file(const std::string& filename, std::vector<char>& buffer);

std::vector<std::string> split_words(const std::string& input_string);
std::vector<std::string> split_lines(const std::string& input_string);

std::string get_mime_type(std::string& filename);

Windsock::Windsock() {
    
    Log.Clear();
}

ClientRequest::ClientRequest() : 
    isReadyToSend(false)
{
}

int Windsock::initiate(void) {
    int state = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (state != 0) 
        printf("WSA failed to initiate\n");
    
    return state;
}

int Windsock::start(void) {
    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Socket failed to initiate\n");
        WSACleanup();
        return -1;
    }
    
    // Bind to port 80
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(80);
    
    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Bind failed.\n");
        closesocket(serverSocket);
        WSACleanup();
        return -2;
    }
    
    // Listen for connections
    listen(serverSocket, 1);
    printf("Server ready\n");
    return 0;
}

int Windsock::checkRequests(void) {
    int clientLen = sizeof(client);
    
    // Accept a connection
    clientSocket = accept(serverSocket, (struct sockaddr *)&client, &clientLen);
    if (clientSocket == INVALID_SOCKET) {
        printf("Accept failed.\n");
        return -1;
    }
    
    // Get the client IP, host and the port number
    char* clientIP = inet_ntoa(client.sin_addr);
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];
    ZeroMemory(host, NI_MAXHOST);
    ZeroMemory(port, NI_MAXSERV);
    getnameinfo((sockaddr*)&client, clientLen, host, NI_MAXHOST, port, NI_MAXSERV, 0);
    
    // Clear the incoming buffer
    std::string buffer;
    buffer.resize(2048);
    memset((void*)buffer.c_str(), ' ', 2048);
    
    // Read the received data into the buffer
    int bytesReceived = recv(clientSocket, (char*)buffer.c_str(), buffer.size() - 1, 0);
    if (bytesReceived > 0) {
        // Split the request lines
        std::vector<std::string> lines = split_lines(buffer);
        std::vector<std::string> request = split_words(lines[0]);
        
        std::vector<std::string> ipaddess = split_words(lines[1]);
        
        // Submit the request
        ClientRequest* clientReq = new(ClientRequest);
        clientReq->host      = host;
        clientReq->port      = port;
        clientReq->ipaddress = clientIP;
        
        // Remove the leading slash
        
        // Default to the index
        if (request[1] == "/") 
            request[1] = "/index.html";
        
        clientReq->type = request[0];
        clientReq->socket = clientSocket;
        clientReq->request = request[1];
        clientReq->mime = get_mime_type(request[1]);
        
        clientReq->isReadyToSend = false;
        mClients.push_back(clientReq);
        
        std::cout << ipaddess[1] << " request  " << request[1] << "\n";
        
        Log.Write(clientReq->ipaddress);
        Log.Write(" : ");
        Log.Write(clientReq->port);
        Log.Write(" << " + clientReq->request);
        Log.WriteLn();
        
    }
    return 0;
}


int Windsock::processRequests(void) {
    for (unsigned int i=0; i < mClients.size(); i++) {
        ClientRequest* request = mClients[i];
        
        if (!request->isReadyToSend) 
            continue;
        
        std::cout << request->ipaddress << " return   " << request->request << "\n";
        
        // Log the incoming connection request
        Log.Write(request->ipaddress);
        Log.Write(" : ");
        Log.Write(request->port);
        Log.Write(" >> " + request->request);
        Log.WriteLn();
        
        // Get mime file type
        std::string httpResponseBase = GenerateStatusLine("200", IntToString(request->buffer.size()), request->mime, "close");
        std::string httpResponse = httpResponseBase + request->buffer;
        
        // Send response
        send(request->socket, httpResponse.c_str(), httpResponse.size(), 0);
        
        // Terminate the connection
        closesocket(request->socket);
        mClients.erase( mClients.begin() + i );
        
        break;
    }
    return mClients.size();
}


ClientRequest* Windsock::getClientRequest(unsigned int index) {
    return mClients[index];
}

int Windsock::removeClientRequest(unsigned int index) {
    if (index >= mClients.size()) 
        return -1;
    delete(mClients[index]);
    mClients.erase( mClients.begin() + index );
    return 0;
}

unsigned int Windsock::getNumberOfClientRequests(void) {
    return mClients.size();
}


void Windsock::Shutdown(void) {
    closesocket(serverSocket);
    WSACleanup();
}

std::string Windsock::GenerateStatusLine(std::string statusCode, std::string contentLength, std::string contentType, std::string requestedConnectionState) {
    std::string headerLine;
    headerLine  = "HTTP/1.1 "+statusCode+"\r\n";
    headerLine += "Server: WindSock/0.9.0\r\n";
    headerLine += "Content-Type: "+contentType+"\r\n";
    headerLine += "Content-Length: "+contentLength+"\r\n";
    headerLine += "Connection: "+requestedConnectionState+"\r\n\r\n";
    return headerLine;
}





std::vector<std::string> split_words(const std::string& input_string) {
    std::istringstream iss(input_string);
    std::vector<std::string> words;
    std::string word;
    while (iss >> word) {
        std::transform(word.begin(), word.end(), word.begin(),
            [](unsigned char c) { return std::tolower(c); });
        words.push_back(word);
    }
    return words;
}

std::vector<std::string> split_lines(const std::string& input_string) {
    std::vector<std::string> lines;
    std::string current_line;
    for (char c : input_string) {
        if (c == '\n') {
            std::transform(current_line.begin(), current_line.end(), current_line.begin(),
                [](unsigned char ch) { return std::tolower(ch); });
            lines.push_back(current_line);
            current_line.clear();
        } else {
            current_line += c;
        }
    }
    // Add the last line if it wasn't followed by a newline
    if (!current_line.empty()) {
        std::transform(current_line.begin(), current_line.end(), current_line.begin(),
            [](unsigned char ch) { return std::tolower(ch); });
        lines.push_back(current_line);
    }
    return lines;
}

bool ends_with(const std::string& value, const std::string& ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::string get_mime_type(std::string& filename) {
    if (ends_with(filename, ".html") || ends_with(filename, ".htm")) return "text/html";
    if (ends_with(filename, ".jpg") || ends_with(filename, ".jpeg")) return "image/jpeg";
    if (ends_with(filename, ".css"))   return "text/css";
    if (ends_with(filename, ".js"))    return "application/javascript";
    if (ends_with(filename, ".png"))   return "image/png";
    if (ends_with(filename, ".gif"))   return "image/gif";
    if (ends_with(filename, ".svg"))   return "image/svg+xml";
    return "application/octet-stream"; // fallback
}


bool fetch_file(const std::string& filename, std::vector<char>& buffer) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) 
        return false;
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    buffer.resize(size);
    if (!file.read(buffer.data(), size)) 
        return false;
    
    return true;
}


