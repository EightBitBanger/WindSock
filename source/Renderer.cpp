#include "Renderer.h"

HTMLRenderer::HTMLRenderer() :
    buffer("")
{
    
}



void HTMLRenderer::dividerLine(void) {
    buffer += "<hr>";
}

void HTMLRenderer::newLine(void) {
    buffer += "<br></br>";
}

void HTMLRenderer::header(std::string title, std::string color) {
    buffer += "<html>\r\n";
    buffer += "<head><title>" + title + "</title></head>\r\n";
    buffer += "<body bgcolor=" + color + ">\r\n";
}

void HTMLRenderer::beginDiv(void) {
    buffer += "<div>";
}

void HTMLRenderer::beginTagSize(std::string tag, unsigned int size) {
    std::string textSz = IntToString(size);
    buffer += "<" + tag + textSz + ">\r\n";
}

void HTMLRenderer::beginTagStyle(std::string tag, std::string color, unsigned int size) {
    std::string textSz = IntToString(size);
    buffer += "<" + tag + textSz + " style=\"color:" + color + "\">\r\n";
}

void HTMLRenderer::beginTag(std::string tag) {
    buffer += "<" + tag + ">\r\n";
}

void HTMLRenderer::text(std::string tag, std::string text, unsigned int size) {
    std::string textSz = IntToString(size);
    buffer += "  <" + tag + textSz + ">" + text + "</" + tag + textSz + ">\r\n";
}

void HTMLRenderer::link(std::string tag, std::string text, std::string link, std::string color) {
    buffer += "<" + tag + " href=\"" + link + " \" style=\"color:" + color + "\"> " + text + " </" + tag + ">";
}

void HTMLRenderer::image(std::string textLink, unsigned int width, unsigned int height) {
    if (width > 0) {
        std::string widthStr = IntToString(width);
        std::string heightStr = IntToString(height);
        buffer += "  <img src=\"" + textLink + "\" alt=\"img\" width = \"" + widthStr + "\" height = \"" + heightStr + "\">\r\n";
    } else {
        buffer += "  <img src=\"" + textLink + "\" alt=\"img\">\r\n";
    }
}

void HTMLRenderer::endDiv(void) {
    buffer += "</div>";
}

void HTMLRenderer::endTag(std::string tag) {
    buffer += "</" + tag + ">\r\n";
}

void HTMLRenderer::endTagSize(std::string tag, unsigned int size) {
    std::string textSz = IntToString(size);
    buffer += "</" + tag + textSz + ">\r\n";
}

void HTMLRenderer::footer(void) {
    buffer += "\r\n</body>\r\n";
    buffer += "</html>\r\n";
}


std::string HTMLRenderer::bufferFlush(void) {
    return buffer;
}

void HTMLRenderer::bufferClear(void) {
    buffer = "";
}

