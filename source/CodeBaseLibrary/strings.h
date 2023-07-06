#ifndef STRING_FUNCTIONS
#define STRING_FUNCTIONS

#include <sstream>

#include <string>
#include <vector>

/// Explode the string by the given delimiter into an array of strings.
std::vector<std::string> StringExplode(std::string String, const char Char);

/// Replace the first instances of "from" in the string "str" with the string "to".
std::string StringReplace(std::string str, std::string from, std::string to);

/// Replace all instances of "from" in the string "str" with the string "to".
std::string StringReplaceAll(std::string str, std::string from, std::string to);

/// Returns the file name from a path string.
std::string StringGetNameFromFilename(std::string FilePath);

/// Returns the file name without an extension from a path string.
std::string StringGetNameFromFilenameNoExt(std::string FilePath);

/// Returns the file extension only.
std::string StringGetExtFromFilename(std::string FilePath);

#endif
