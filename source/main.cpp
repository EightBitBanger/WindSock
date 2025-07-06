#include <iostream>
#include <sstream>
#include <algorithm>

#include <vector>
#include <string>
#include <fstream>

#include "windsock.h"
#include "Renderer.h"
#include "Filesystem.h"

int StringToInt(std::string str);
std::string IntToString(int value);

std::string contentDirectory = "content";
std::string searchDirectory  = "search";

int main() {
    
    // Load configuration settings
    std::vector<char> configBuffer;
    if (fetch_file("config.txt", configBuffer) == true) {
        std::string configString;
        configString.resize( configBuffer.size() );
        
        memcpy((void*)configString.c_str(), configBuffer.data(), configBuffer.size());
        
        std::vector<std::string> configLines = split_lines(configString);
        for (unsigned int i=0; i < configLines.size(); i++) {
            if (configLines[i].find("=") == std::string::npos) 
                continue;
            std::string key = configLines[i];
            std::string value = configLines[i];
            
            //key.erase(key.find("="), key.find("=") - (key.size()));
            //value.erase(0, value.find("=")+1);
            
            //if (key == "site")   contentDirectory = value.c_str();
            //if (key == "search") searchDirectory = value.c_str();
            
            //std::cout << contentDirectory.c_str() << std::endl;
        }
        
    }
    
    // Initiate the server
    Windsock sock;
    sock.initiate();
    
    
    std::string messagingBoard;
    
    sock.start();
    
    while (1) {
        
        sock.processRequests();
        sock.checkRequests();
        
        unsigned int numberOfRequests = sock.getNumberOfClientRequests();
        for (unsigned int i=0; i < numberOfRequests; i++) {
            ClientRequest* request = sock.getClientRequest(i);
            
            // Fetch the file
            std::vector<char> fileBuffer;
            if (fetch_file(contentDirectory + request->request, fileBuffer) == true) {
                request->buffer.resize( fileBuffer.size() );
                
                memcpy((void*)request->buffer.c_str(), fileBuffer.data(), fileBuffer.size());
                
                request->isReadyToSend = true;
                continue;
            }
            
            // Check search request
            std::string requestCheck = request->request;
            requestCheck.erase(9, requestCheck.size()-1);
            std::string compareString = "/?search=";
            
            if (strcmp(compareString.c_str(), requestCheck.c_str()) == 0) {
                requestCheck = request->request;
                requestCheck.erase(0, 9);
                
                // Generate search results
                
                HTMLRenderer page;
                page.header("Search results", "#010409");
                
                page.beginTagStyle("h", "#767676", 1);
                
                page.text("p", "Results for '" + requestCheck + "'", 1);
                page.endTagSize("h", 1);
                page.newLine();
                
                page.beginTagStyle("h", "#767676", 4);
                
                // Get a listing of the files in the 
                
                std::vector<std::string> sourceList = DirectoryGetFileList("*.*");
                std::vector<std::string> finalList;
                // Get the files to be included in the final listing
                for (unsigned int f=0; f < sourceList.size(); f++) {
                    std::string filename = sourceList[f];
                    
                    if (FileCheckExists(filename) == true) 
                        finalList.push_back(filename);
                    
                }
                
                
                for (unsigned int f=0; f < finalList.size(); f++) {
                    std::string filename = finalList[f];
                    
                    page.dividerLine();
                    
                    page.buffer += "<div style=\"display: flex; align-items: flex-start;\">";
                    page.image("image.jpg", 55, 50, 4);
                    
                    page.buffer += "<div>";
                    
                    
                    page.beginTagStyle("p", "#e0e0e0", 1);
                    page.textMargin("h", filename, 2, 4);
                    page.endTagSize("p", 1);
                    
                    page.textMargin("h", "Additional details", 5, 2);
                    page.textMargin("h", "some more details", 5, 2);
                    
                    page.buffer += "</div>";
                    page.buffer += "</div>";
                    
                    page.newLine();
                    
                }
                
                
                page.endTagSize("h", 4);
                
                page.footer();
                
                
                
                // Finalize the page
                request->buffer = page.bufferFlush();
                
                request->mime = "text/html";
                request->isReadyToSend = true;
                
                continue;
            }
            
            
            // 404 page
            
            HTMLRenderer page;
            page.header("404 - page not found", "#010409");
            
            page.beginTagStyle("h", "#555555", 1);
            page.beginTag("center");
            
            page.text("h", "404", 3);
            
            page.endTagSize("h", 2);
            
            page.footer();
            
            // Finalize the page
            request->buffer = page.bufferFlush();
            
            request->mime = "text/html";
            request->isReadyToSend = true;
            
        }
        
    }
    
    sock.Shutdown();
    return 0;
}


















