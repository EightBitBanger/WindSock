#include "filesystem.h"

bool DirectoryCheckExists(std::string directoryName) {
    DWORD dwAttrib = GetFileAttributes( (LPCTSTR)directoryName.data() );
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool FileCheckExists(std::string filename) {
    std::ifstream fStream(filename.c_str(), std::ios::binary | std::ios::ate);
    if (!fStream.is_open()) 
        return false;
    fStream.close();
    return true;
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
