#ifndef HTTPREDIRECTION_HPP
#define HTTPREDIRECTION_HPP

#include <string>
#include <sstream>

class HttpRedirection
{
public:
    HttpRedirection();
    HttpRedirection(const HttpRedirection& other);
    ~HttpRedirection();

    HttpRedirection& operator=(const HttpRedirection& other);
    static std::string getStatusMessage(int code);
    static std::string redir(int code, const std::string& location);
    static std::string basic(const std::string& body, const std::string& newPath);
};

#endif
