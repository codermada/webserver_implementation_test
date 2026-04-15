#include "Header.hpp"
#include "ExceptionHandler.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>

Header::Header()
    : header(""), method(""), path(""), queryString(""), contentType(""),
      boundary(""), contentLength(0), isChunked(false)
{
}

Header::Header(const std::string& request, int max_body_size)
    : header(request), method(""), path(""), queryString(""), contentType(""),
      boundary(""), contentLength(0), isChunked(false)
{
    std::istringstream iss(request);
    std::string        line;

    // Parse request line
    if (!std::getline(iss, line))
        throw ExceptionHandler::BadRequest("Empty request");

    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);

    std::istringstream firstLine(line);
    std::string        version;
    std::string        rawPath;
    std::string        rawUri;

    if (!(firstLine >> method >> rawUri >> version))
        throw ExceptionHandler::BadRequest("Malformed request line");

    // Validate method
    if (method != "GET" && method != "POST" && method != "DELETE")
        throw ExceptionHandler::MethodNotAllowed("Unsupported HTTP method");

    // Validate HTTP version
    if (version != "HTTP/1.1" && version != "HTTP/1.0")
        throw ExceptionHandler::HttpVersionNotSupported(
            "Unsupported HTTP version");

    // Parse path and query string from rawUri
    std::string::size_type queryPos = rawUri.find('?');
    if (queryPos != std::string::npos)
    {
        rawPath = rawUri.substr(0, queryPos);

        // Check for multiple ? characters
        if (rawUri.find('?', queryPos + 1) != std::string::npos)
            throw ExceptionHandler::BadRequest("Multiple ? in URI");

        queryString = rawUri.substr(queryPos + 1);

        // Validate query string doesn't start with ? or &
        if (!queryString.empty() &&
            (queryString[0] == '?' || queryString[0] == '&'))
            throw ExceptionHandler::BadRequest("Malformed query string");

        // Validate query string keys and values
        if (!isValidQueryString(queryString))
            throw ExceptionHandler::BadRequest("Malformed query string");
    }
    else
    {
        rawPath     = rawUri;
        queryString = "";
    }

    // Normalize path only (remove duplicate slashes and resolve . and ..)
    path = normalizePath(rawPath);

    // Validate URI length (basic protection)
    if (path.length() > 2048)
        throw ExceptionHandler::URITooLong("URI too long");

    // Validate path format
    if (path.empty() || path[0] != '/')
        throw ExceptionHandler::BadRequest("Invalid URI");

    // Parse headers
    while (std::getline(iss, line))
    {
        if (line == "\r" || line == "")
            break;

        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        // Content-Type
        if (line.find("Content-Type:") == 0)
        {
            contentType = line.substr(13);
            while (!contentType.empty() && contentType[0] == ' ')
                contentType.erase(0, 1);

            std::string            key = "boundary=";
            std::string::size_type pos = contentType.find(key);
            if (pos != std::string::npos)
                boundary = contentType.substr(pos + key.length());
        }

        // Content-Length
        else if (line.find("Content-Length:") == 0)
        {
            std::string lenStr = line.substr(15);
            while (!lenStr.empty() && lenStr[0] == ' ')
                lenStr.erase(0, 1);

            if (lenStr.empty())
                throw ExceptionHandler::BadRequest("Missing Content-Length");

            for (size_t i = 0; i < lenStr.size(); i++)
            {
                if (!std::isdigit(lenStr[i]))
                    throw ExceptionHandler::BadRequest(
                        "Invalid Content-Length");
            }

            contentLength = std::atoi(lenStr.c_str());

            if (contentLength < 0)
                throw ExceptionHandler::BadRequest("Negative Content-Length");
        }

        // Transfer-Encoding
        else if (line.find("Transfer-Encoding:") == 0)
        {
            std::string value = line.substr(18);
            while (!value.empty() && value[0] == ' ')
                value.erase(0, 1);

            if (value == "chunked")
                isChunked = true;
        }
    }

    // Conflict: both chunked and content-length
    if (isChunked && contentLength > 0)
        throw ExceptionHandler::BadRequest("Conflicting headers");

    // POST rules
    if (isPost())
    {
        // No body info at all
        if (!isChunked && contentLength == 0)
            throw ExceptionHandler::LengthRequired(
                "POST requires Content-Length or chunked encoding");

        // Payload too large
        if (contentLength > max_body_size)
            throw ExceptionHandler::PayloadTooLarge("Payload too large");
    }

    // Multipart validation
    if (isMultipart() && boundary.empty())
        throw ExceptionHandler::BadRequest("Missing multipart boundary");

    // Optional: reject unsupported content types
    if (isPost() && !contentType.empty())
    {
        if (contentType.find("multipart/form-data") == std::string::npos &&
            contentType.find("application/json") == std::string::npos &&
            contentType.find("application/x-www-form-urlencoded") ==
                std::string::npos &&
            contentType.find("text/plain") == std::string::npos)
        {
            throw ExceptionHandler::UnsupportedMediaType(
                "Unsupported Content-Type");
        }
    }
}

Header::Header(const Header& other)
{
    *this = other;
}

Header::~Header() {}

Header& Header::operator=(const Header& other)
{
    if (this != &other)
    {
        header        = other.header;
        method        = other.method;
        path          = other.path;
        queryString   = other.queryString;
        contentType   = other.contentType;
        boundary      = other.boundary;
        contentLength = other.contentLength;
        isChunked     = other.isChunked;
    }
    return *this;
}

std::string Header::getMethod() const
{
    return method;
}

std::string Header::getPath() const
{
    return path;
}

std::string Header::getQueryString() const
{
    return queryString;
}

std::string Header::getContentType() const
{
    return contentType;
}

std::string Header::getBoundary() const
{
    return boundary;
}

int Header::getContentLength() const
{
    return contentLength;
}

bool Header::getIsChunked() const
{
    return isChunked;
}

bool Header::isPost() const
{
    return method == "POST";
}

bool Header::isMultipart() const
{
    return contentType.find("multipart/form-data") != std::string::npos;
}

bool Header::isValidQueryString(const std::string& qs) const
{
    if (qs.empty())
        return true;

    // Check for invalid characters at start
    if (qs[0] == '?' || qs[0] == '&')
        return false;

    // Check for empty segments (&&)
    if (qs.find("&&") != std::string::npos)
        return false;

    // Check for empty segments (&&&)
    if (qs.find("&&&") != std::string::npos)
        return false;

    // Validate each parameter
    std::stringstream ss(qs);
    std::string       param;

    while (std::getline(ss, param, '&'))
    {
        // Skip trailing & (empty param at end)
        if (param.empty() && ss.eof())
            continue;

        if (param.empty())
            return false;

        // Check for invalid characters at start of parameter
        if (param[0] == '?' || param[0] == '&' || param[0] == '=')
            return false;

        // Parse key and value
        std::string            key;
        std::string            value;
        std::string::size_type eqPos = param.find('=');

        if (eqPos != std::string::npos)
        {
            key   = param.substr(0, eqPos);
            value = param.substr(eqPos + 1);
        }
        else
        {
            key   = param;
            value = "";
        }

        // Validate key
        if (!isValidQueryKey(key))
            return false;

        // Validate value
        if (!isValidQueryValue(value))
            return false;

        // Check for multiple equals signs
        size_t equalCount = 0;
        for (size_t i = 0; i < param.size(); ++i)
        {
            if (param[i] == '=')
                equalCount++;
        }
        if (equalCount > 1)
            return false;
    }

    return true;
}

bool Header::isValidQueryKey(const std::string& key) const
{
    if (key.empty())
        return false;

    // Valid characters for key: alphanumeric, _, -, ., ~ (and percent-encoded)
    for (size_t i = 0; i < key.size(); ++i)
    {
        char c = key[i];
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' ||
            c == '-' || c == '.' || c == '~')
        {
            continue;
        }
        // Allow percent-encoding
        if (c == '%' && i + 2 < key.size() && std::isxdigit(key[i + 1]) &&
            std::isxdigit(key[i + 2]))
        {
            i += 2;
            continue;
        }
        return false;
    }

    return true;
}

bool Header::isValidQueryValue(const std::string& value) const
{
    // Empty value is allowed
    if (value.empty())
        return true;

    // Valid characters for value: alphanumeric, _, -, ., ~, space, and safe
    // special chars
    for (size_t i = 0; i < value.size(); ++i)
    {
        char c = value[i];
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' ||
            c == '-' || c == '.' || c == '~' || c == ' ' || c == '/' ||
            c == ':' || c == '@' || c == '!' || c == '$' || c == '\'' ||
            c == '(' || c == ')' || c == '*' || c == '+' || c == ',')
        {
            continue;
        }
        // Allow percent-encoding
        if (c == '%' && i + 2 < value.size() && std::isxdigit(value[i + 1]) &&
            std::isxdigit(value[i + 2]))
        {
            i += 2;
            continue;
        }
        return false;
    }

    return true;
}

std::string Header::normalizePath(const std::string& rawPath) const
{
    // Handle empty path
    if (rawPath.empty())
        return "/";

    std::string              result;
    std::vector<std::string> segments;
    std::stringstream        ss(rawPath);
    std::string              segment;
    bool                     isAbsolute = (rawPath[0] == '/');

    // Split path by '/'
    while (std::getline(ss, segment, '/'))
    {
        if (segment.empty() || segment == ".")
            continue;
        else if (segment == "..")
        {
            if (!segments.empty())
                segments.pop_back();
        }
        else
        {
            segments.push_back(segment);
        }
    }

    // Rebuild normalized path
    if (isAbsolute)
        result = "/";

    for (size_t i = 0; i < segments.size(); ++i)
    {
        if (i > 0 || !isAbsolute)
            result += "/";
        result += segments[i];
    }

    // Handle root path
    if (result.empty())
        result = "/";

    // Preserve trailing slash if original had it
    if (rawPath.size() > 1 && rawPath[rawPath.size() - 1] == '/' &&
        result != "/")
        result += "/";

    return result;
}