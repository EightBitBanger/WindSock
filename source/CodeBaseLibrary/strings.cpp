#include <iostream>
#include "strings.h"


std::vector<std::string> StringExplode(std::string String, const char Char) {
	
    std::vector<std::string> result;
    std::istringstream iss(String);
    
    for (std::string token; std::getline(iss, token, Char); ) {
        
        if (std::move(token) != "") {
            result.push_back(std::move(token));
        }
        
    }
    
    return result;
}

std::string StringReplace(std::string str, std::string from, std::string to) {
    size_t pos = str.find(from, 0);
    if (pos == std::string::npos) 
        return str;
    str.replace(pos, from.length(), to);
    return str;
}

std::string StringReplaceAll(std::string str, std::string from, std::string to) {
    size_t pos = str.find(from);
    while(pos != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos = str.find(from, pos);
    }
    return str;
}

std::string StringGetNameFromFilename(std::string FilePath) {
    std::vector<std::string> pathParts = StringExplode(FilePath, '/');
    return pathParts[pathParts.size()-1];
}

std::string StringGetNameFromFilenameNoExt(std::string FilePath) {
    std::vector<std::string> pathParts = StringExplode(FilePath, '/');
    std::vector<std::string> name = StringExplode(pathParts[pathParts.size()-1], '.');
    return name[0];
}

std::string StringGetExtFromFilename(std::string FilePath) {
	std::vector<std::string> pathParts = StringExplode(FilePath, '/');
    std::vector<std::string> name = StringExplode(pathParts[pathParts.size()-1], '.');
    if (name.size() == 2) 
        return name[1];
    return "";
}









