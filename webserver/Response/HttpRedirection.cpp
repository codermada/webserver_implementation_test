#include "HttpRedirection.hpp"

HttpRedirection::HttpRedirection() {}

HttpRedirection::HttpRedirection(const HttpRedirection& other)
{
    (void)other;
}

HttpRedirection::~HttpRedirection() {}

HttpRedirection& HttpRedirection::operator=(const HttpRedirection& other)
{
    if (this != &other)
    {
        // copy members here
        (void)other;
    }
    return *this;
}

std::string HttpRedirection::getStatusMessage(int code)
{
    switch (code)
    {
        // 3xx Redirection
        case 300:
            return "Multiple Choices";
        case 301:
            return "Moved Permanently";
        case 302:
            return "Found";
        case 303:
            return "See Other";
        case 304:
            return "Not Modified";
        case 305:
            return "Use Proxy";
        case 307:
            return "Temporary Redirect";
        case 308:
            return "Permanent Redirect";

        default:
            return "Unknown Status";
    }
}

std::string HttpRedirection::redir(int code, const std::string& location)
{
    std::ostringstream response;

    response << "HTTP/1.1 " << code << " " << getStatusMessage(code) << "\r\n"
             << "Location: " << location << "\r\n"
             << "Connection: close\r\n"
             << "Content-Length: 0\r\n"
             << "\r\n";
    return response.str();
}

std::string HttpRedirection::basic(const std::string& body,
                                   const std::string& newPath)
{
    std::ostringstream oss;

    oss << "HTTP/1.1 302 Found\r\n"
        << "Location: " << newPath << "\r\n"
        << "Content-Type: text/html\r\n"
        << "Connection: close\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "\r\n"
        << body;
    return oss.str();
}