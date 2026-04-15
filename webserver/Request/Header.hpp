#ifndef HEADER_HPP
#define HEADER_HPP

#include <string>

#include "ExceptionHandler.hpp"
#include "Validator.hpp"
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <map>

class Header
{
private:
    std::string header;
    std::string method;
    std::string path;
    std::string queryString;
    std::string contentType;
    std::string boundary;
    int         contentLength;
    bool        isChunked;
    std::string normalizePath(const std::string& rawPath) const;

public:
    Header();
    Header(const std::string& request, int max_body_size);
    Header(const Header& other);
    ~Header();

    Header& operator=(const Header& other);

    std::string getMethod() const;
    std::string getPath() const;
    std::string getContentType() const;
    std::string getBoundary() const;
    int         getContentLength() const;
    bool        getIsChunked() const;

    bool        isPost() const;
    bool        isMultipart() const;
    bool isValidQueryString(const std::string& qs) const;
    bool isValidQueryKey(const std::string& key) const;
    bool isValidQueryValue(const std::string& value) const;
    std::string getQueryString() const;
};

#endif