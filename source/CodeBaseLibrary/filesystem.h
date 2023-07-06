#ifndef _FILE_SYSTEM__
#define _FILE_SYSTEM__

#include <windows.h>
#include <string>
#include <vector>
#include <fstream>

bool DirectoryCheckExists(std::string directoryName);

int FileCheckExists(std::string filename);

std::vector<std::string> DirectoryGetFileList(std::string path);

int FileGetSize(std::string filename);

int FileLoadText(std::string filename, std::string& buffer);

int FileLoadRaw(std::string filename, char* buffer, unsigned int bufferSize);

#endif
