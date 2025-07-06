#include "Renderer.h"
#include <sstream>

int StringToInt(std::string str);
std::string IntToString(int value);


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
    buffer += "<!DOCTYPE html>\r\n";
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
    buffer += "  <"+tag+textSz+">" + text + "</" + tag + textSz + ">\r\n";
}

void HTMLRenderer::textMargin(std::string tag, std::string text, unsigned int size, unsigned int margin) {
    std::string marginString = IntToString(margin);
    std::string textSz = IntToString(size);
    buffer += "  <"+tag+textSz+" style=\"margin: "+marginString+"px 0;\">" + text + "</"+tag+textSz+">\r\n";
}


void HTMLRenderer::link(std::string tag, std::string text, std::string link, std::string color) {
    buffer += "<"+tag+ " href=\"" + link + " \" style=\"color:" + color + "\"> " + text + " </" + tag + ">";
}

void HTMLRenderer::image(std::string textLink, unsigned int width, unsigned int height, unsigned int margin) {
    if (width > 0) {
        std::string widthStr = IntToString(width);
        std::string heightStr = IntToString(height);
        std::string marginStr = IntToString(margin);
        buffer += "  <img src=\"" + textLink + "\" alt=\"img\" style=\"width: "+widthStr+"px; height: "+heightStr+"px; margin-right: "+marginStr+"px;\">\r\n";
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


// Assistant functions

std::string IntToString(int value) {
    std::stringstream sStream;
    sStream << value;
    return sStream.str();
}

int StringToInt(std::string str) {
    int value;
    std::stringstream(str) >> value;
    return value;
}


