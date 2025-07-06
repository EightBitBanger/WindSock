#ifndef _FILESYSTEM_FUNCTIONS_
#define _FILESYSTEM_FUNCTIONS_
#include <vector>
#include <string>
#include <fstream>

#include <windows.h>

bool DirectoryCheckExists(std::string directoryName);

bool FileCheckExists(std::string filename);

std::vector<std::string> DirectoryGetFileList(std::string path);

int FileGetSize(std::string filename);

#endif
