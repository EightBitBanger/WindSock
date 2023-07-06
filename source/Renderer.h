#ifndef _RENDERER_SERVER_
#define _RENDERER_SERVER_

#include <string>
#include "codebaselibrary/types.h"



class HTMLRenderer {
    
public:
    
    HTMLRenderer();
    
    // HTML rendering
    void header(std::string title, std::string color);
    void footer(void);
    
    void dividerLine(void);
    void newLine(void);
    
    void beginTag(std::string tag);
    void beginDiv(void);
    void beginTagSize(std::string tag, unsigned int size);
    void beginTagStyle(std::string tag, std::string color, unsigned int size);
    
    void text(std::string tag, std::string text, unsigned int size);
    void link(std::string tag, std::string text, std::string link, std::string color);
    void image(std::string textLink, unsigned int width, unsigned int height);
    
    void endTag(std::string tag);
    void endDiv(void);
    void endTagSize(std::string tag, unsigned int size);
    
    std::string bufferFlush(void);
    void bufferClear(void);
    
private:
    
    std::string buffer;
    
    
};

#endif
