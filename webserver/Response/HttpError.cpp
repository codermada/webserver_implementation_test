#include "HttpError.hpp"

HttpError::HttpError() {}

HttpError::HttpError(const HttpError& other)
{
    (void)other;
}

HttpError::~HttpError() {}

HttpError& HttpError::operator=(const HttpError& other)
{
    if (this != &other)
    {
        (void)other;
    }
    return (*this);
}

std::string HttpError::getStatusMessage(int code)
{
    switch (code)
    {
        // 4xx Client Errors
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 402:
            return "Payment Required";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 406:
            return "Not Acceptable";
        case 407:
            return "Proxy Authentication Required";
        case 408:
            return "Request Timeout";
        case 409:
            return "Conflict";
        case 410:
            return "Gone";
        case 411:
            return "Length Required";
        case 412:
            return "Precondition Failed";
        case 413:
            return "Payload Too Large";
        case 414:
            return "URI Too Long";
        case 415:
            return "Unsupported Media Type";
        case 416:
            return "Range Not Satisfiable";
        case 417:
            return "Expectation Failed";
        case 418:
            return "I'm a teapot"; // optional but fun
        case 426:
            return "Upgrade Required";

        // 5xx Server Errors
        case 500:
            return "Internal Server Error";
        case 501:
            return "Not Implemented";
        case 502:
            return "Bad Gateway";
        case 503:
            return "Service Unavailable";
        case 504:
            return "Gateway Timeout";
        case 505:
            return "HTTP Version Not Supported";

        default:
            return "Unknown Status";
    }
}

std::string HttpError::error(int code, const std::string& errorPath)
{
    std::string path = errorPath;
    std::string body;

    try
    {
        if (Validator::isDirectory(errorPath))
            throw std::exception();
        body = File::getFileContent(path);
    }
    catch (const std::exception& e)
    {
        try
        {
            std::ostringstream oss_;
            oss_ << "./errors/" << code << ".html";
            path = oss_.str();

            body = File::getFileContent(path);
        }
        catch (const std::exception& e)
        {
            std::ostringstream fallback;
            fallback << "<html><body><h1>" << code
                     << " Error</h1></body></html>";
            body = fallback.str();
        }
    }
    std::string contentType = "text/html; charset=UTF-8";

    std::ostringstream response;
    response << "HTTP/1.1 " << code << " " << getStatusMessage(code) << "\r\n"
             << "Content-Type: " << contentType << "\r\n"
             << "Connection: close\r\n"
             << "Content-Length: " << body.size() << "\r\n"
             << "\r\n"
             << body;

    return response.str();
}