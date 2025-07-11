#ifndef EVENT_LOGGING
#define EVENT_LOGGING

#include <string>

#define  EVENT_LOG_FILENAME   "log.txt"


class Logger {
    
public:
    
    /// Write a string to the log file.
    void Write(std::string LogString);
    
    /// Write a blank line to the log file.
    void WriteLn(void);
    
    /// Delete the log file.
    void Clear(void);
    
};

#endif
