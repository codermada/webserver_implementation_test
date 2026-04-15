#ifndef HTTPSUCCESS_HPP
#define HTTPSUCCESS_HPP

#include <string>
#include <sstream>

#include "File.hpp"

class HttpSuccess
{
public:
    HttpSuccess();
    HttpSuccess(const HttpSuccess& other);
    ~HttpSuccess();

    HttpSuccess& operator=(const HttpSuccess& other);

    static std::string getStatusMessage(int code);
    static std::string getMimeType(const std::string& path);
    static std::string success(int code, const std::string& succesFolder);
    static std::string basic(const std::string& body, const std::string& path);
};

#endif
