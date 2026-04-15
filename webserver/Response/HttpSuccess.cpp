#include "HttpSuccess.hpp"

HttpSuccess::HttpSuccess() {}

HttpSuccess::HttpSuccess(const HttpSuccess& other)
{
    (void)other;
}

HttpSuccess::~HttpSuccess() {}

HttpSuccess& HttpSuccess::operator=(const HttpSuccess& other)
{
    if (this != &other)
    {
        // copy members here
        (void)other;
    }
    return *this;
}

std::string HttpSuccess::getMimeType(const std::string& path)
{
    std::string::size_type pos = path.rfind('.');
    if (pos == std::string::npos)
        return "text/html; charset=UTF-8";
    std::string ext = path.substr(pos);

    // Images
    if (ext == ".jpg" || ext == ".jpeg")
        return "image/jpeg";
    if (ext == ".png")
        return "image/png";
    if (ext == ".gif")
        return "image/gif";
    if (ext == ".ico")
        return "image/x-icon";
    if (ext == ".svg" || ext == ".svgz")
        return "image/svg+xml";
    if (ext == ".webp")
        return "image/webp";
    if (ext == ".bmp")
        return "image/x-ms-bmp";
    if (ext == ".tif" || ext == ".tiff")
        return "image/tiff";
    if (ext == ".avif")
        return "image/avif";

    // Videos
    if (ext == ".mp4")
        return "video/mp4";
    if (ext == ".webm")
        return "video/webm";
    if (ext == ".avi")
        return "video/x-msvideo";
    if (ext == ".mov")
        return "video/quicktime";
    if (ext == ".mpeg" || ext == ".mpg")
        return "video/mpeg";
    if (ext == ".ts")
        return "video/mp2t";
    if (ext == ".flv")
        return "video/x-flv";
    if (ext == ".m4v")
        return "video/x-m4v";
    if (ext == ".wmv")
        return "video/x-ms-wmv";
    if (ext == ".3gp" || ext == ".3gpp")
        return "video/3gpp";

    // Audio
    if (ext == ".mp3")
        return "audio/mpeg";
    if (ext == ".ogg")
        return "audio/ogg";
    if (ext == ".m4a")
        return "audio/x-m4a";
    if (ext == ".mid" || ext == ".midi" || ext == ".kar")
        return "audio/midi";

    // Text-based
    if (ext == ".html" || ext == ".htm" || ext == ".shtml")
        return "text/html; charset=UTF-8";
    if (ext == ".css")
        return "text/css; charset=UTF-8";
    if (ext == ".xml")
        return "text/xml; charset=UTF-8";
    if (ext == ".txt")
        return "text/plain; charset=UTF-8";
    if (ext == ".js")
        return "application/javascript; charset=UTF-8";
    if (ext == ".json")
        return "application/json; charset=UTF-8";
    if (ext == ".rss")
        return "application/rss+xml";
    if (ext == ".atom")
        return "application/atom+xml";
    if (ext == ".xhtml")
        return "application/xhtml+xml";

    // Documents
    if (ext == ".pdf")
        return "application/pdf";
    if (ext == ".doc")
        return "application/msword";
    if (ext == ".docx")
        return "application/"
               "vnd.openxmlformats-officedocument.wordprocessingml.document";
    if (ext == ".xls")
        return "application/vnd.ms-excel";
    if (ext == ".xlsx")
        return "application/"
               "vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    if (ext == ".ppt")
        return "application/vnd.ms-powerpoint";
    if (ext == ".pptx")
        return "application/"
               "vnd.openxmlformats-officedocument.presentationml.presentation";
    if (ext == ".odt")
        return "application/vnd.oasis.opendocument.text";
    if (ext == ".ods")
        return "application/vnd.oasis.opendocument.spreadsheet";
    if (ext == ".odp")
        return "application/vnd.oasis.opendocument.presentation";

    // Archives / binaries
    if (ext == ".zip")
        return "application/zip";
    if (ext == ".rar")
        return "application/x-rar-compressed";
    if (ext == ".7z")
        return "application/x-7z-compressed";
    if (ext == ".tar")
        return "application/x-tar";
    if (ext == ".gz")
        return "application/gzip";

    // Fonts
    if (ext == ".woff")
        return "font/woff";
    if (ext == ".woff2")
        return "font/woff2";
    if (ext == ".eot")
        return "application/vnd.ms-fontobject";

    // Default binary
    if (ext == ".exe" || ext == ".dll" || ext == ".bin" || ext == ".iso")
        return "application/octet-stream";

    return "text/html; charset=UTF-8";
}

std::string HttpSuccess::getStatusMessage(int code)
{
    switch (code)
    {
        // 1xx Informational
        case 100:
            return "Continue";
        case 101:
            return "Switching Protocols";

        // 2xx Success
        case 200:
            return "OK";
        case 201:
            return "Created";
        case 202:
            return "Accepted";
        case 203:
            return "Non-Authoritative Information";
        case 204:
            return "No Content";
        case 205:
            return "Reset Content";
        case 206:
            return "Partial Content";

        default:
            return "Unknown Status";
    }
}

std::string HttpSuccess::success(int code, const std::string& successFolder)
{
    std::ostringstream oss;
    oss << successFolder << "/" << code << ".html";
    std::string path = oss.str();
    std::string body;
    try
    {
        body = File::getFileContent(path);
    }
    catch (const std::exception& e)
    {
        try
        {
            std::ostringstream oss_;
            oss_ << "./success/" << code << ".html";
            path = oss_.str();
            body = File::getFileContent(path);
        }
        catch (const std::exception& e)
        {
            std::ostringstream fallback;
            fallback << "<html><body><h1>" << code
                     << " Success</h1></body></html>";
            body = fallback.str();
        }
    }
    std::string        contentType = getMimeType(path);
    std::ostringstream response;
    response << "HTTP/1.1 " << code << " " << getStatusMessage(code) << "\r\n"
             << "Content-Type: " << contentType << "\r\n"
             << "Connection: close\r\n"
             << "Content-Length: " << body.size() << "\r\n"
             << "\r\n"
             << body;
    return response.str();
}

std::string HttpSuccess::basic(const std::string& body, const std::string& path)
{
    std::ostringstream oss;
    std::string        contentType = getMimeType(path);
    oss << "HTTP/1.1 200 OK\r\n"
        << "Content-Type: " << contentType << "\r\n"
        << "Connection: close\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "\r\n"
        << body;
    return oss.str();
}