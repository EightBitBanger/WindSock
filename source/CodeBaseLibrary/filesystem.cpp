#include "filesystem.h"

bool DirectoryCheckExists(std::string directoryName) {
    DWORD dwAttrib = GetFileAttributes( (LPCTSTR)directoryName.data() );
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int FileCheckExists(std::string filename) {
    std::ifstream fStream(filename.c_str(), std::ios::binary | std::ios::ate);
    if (!fStream.is_open()) 
        return -1;
    std::streamsize size = fStream.tellg();
    fStream.close();
    return size;
}

std::vector<std::string> DirectoryGetFileList(std::string path) {
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

int FileGetSize(std::string filename) {
    std::ifstream fStream(filename.c_str(), std::ios::binary | std::ios::ate);
    if (!fStream.is_open()) 
        return -1;
    
    std::streamsize size = fStream.tellg();
    
    fStream.seekg(0, std::ios::beg);
    
    fStream.close();
    return size;
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
    
    fStream.close();
    return size;
}

