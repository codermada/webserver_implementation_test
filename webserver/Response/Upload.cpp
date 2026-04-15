#include "Upload.hpp"
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

Upload::Upload(int client_fd)
    : fd(client_fd), headerParsed(false), header(NULL), file(NULL),
      filename(""), fileOpen(false), finished(false), isMultipart(false),
      isChunked(false), chunkedRemaining(0), inChunkedData(false),
      cgiMode(false), fullRawBody("")
{
}

Upload::~Upload()
{
    if (fileOpen && file)
        file->close();
    delete file;
    delete header;
}

int Upload::getFd() const
{
    return fd;
}
bool Upload::isFinished() const
{
    return finished;
}

void Upload::appendData(const char* data, size_t size)
{
    buffer.insert(buffer.end(), data, data + size);
}

int Upload::findBoundary(const std::string& boundary)
{
    if (buffer.size() < boundary.size())
        return -1;
    for (size_t i = 0; i <= buffer.size() - boundary.size(); ++i)
    {
        bool match = true;
        for (size_t j = 0; j < boundary.size(); ++j)
        {
            if (buffer[i + j] != boundary[j])
            {
                match = false;
                break;
            }
        }
        if (match)
            return static_cast<int>(i);
    }
    return -1;
}

// #include <iostream>
// #include <vector>

// void printBuffer(const std::vector<char>& buffer) {
//     for (std::vector<char>::const_iterator it = buffer.begin(); it !=
//     buffer.end(); ++it) {
//         std::cout << *it;
//     }
//     std::cout << std::endl;
// }

std::vector<char> Upload::dechunkData(const std::vector<char>& data,
                                      size_t&                  bytesProcessed)
{
    // if (cgiMode) {
    //     printBuffer(data);
    // }

    std::vector<char> result;
    size_t            pos = 0;
    bytesProcessed        = 0;

    while (pos < data.size())
    {
        if (chunkedRemaining == 0 && !inChunkedData)
        {
            size_t line_end = pos;
            while (line_end < data.size() &&
                   !(line_end + 1 < data.size() && data[line_end] == '\r' &&
                     data[line_end + 1] == '\n'))
            {
                line_end++;
            }

            if (line_end + 1 >= data.size())
            {
                bytesProcessed = pos;
                return result;
            }

            std::string size_line(data.begin() + pos, data.begin() + line_end);
            char*       endptr;
            long        chunk_size = strtol(size_line.c_str(), &endptr, 16);

            pos = line_end + 2;

            if (chunk_size == 0)
            {
                if (pos + 1 < data.size() && data[pos] == '\r' &&
                    data[pos + 1] == '\n')
                {
                    pos += 2;
                }
                bytesProcessed = pos;
                return result;
            }

            chunkedRemaining = chunk_size;
            inChunkedData    = true;
        }

        if (chunkedRemaining > 0)
        {
            size_t to_read = std::min(static_cast<size_t>(chunkedRemaining),
                                      data.size() - pos);
            result.insert(result.end(), data.begin() + pos,
                          data.begin() + pos + to_read);
            pos += to_read;
            chunkedRemaining -= to_read;

            if (chunkedRemaining == 0)
            {
                if (pos + 1 < data.size() && data[pos] == '\r' &&
                    data[pos + 1] == '\n')
                {
                    pos += 2;
                }
                inChunkedData = false;
            }
        }
    }
    bytesProcessed = pos;
    return result;
}

void Upload::process(UnitConf_t config, std::string folder, bool* isUploading)
{

    if (!headerParsed)
    {
        std::string data_str(buffer.begin(), buffer.end());
        size_t      header_end = data_str.find("\r\n\r\n");
        if (header_end == std::string::npos)
            return;

        std::string header_str = data_str.substr(0, header_end);
        header                 = new Header(header_str, config.maxBodySize);
        if (config.redir.find(header->getPath()) != config.redir.end() &&
            config.redir["status"] == "ON")
        {
            finished           = true;
            std::string target = config.redir[header->getPath()];
            if (target == header->getPath())
            {
                throw ExceptionHandler::InternalServerError(
                    "Redirect loop detected");
            }
        }
        if (!(config.methods & 2))
        {
            if (header->getMethod() == "POST")
            {
                throw ExceptionHandler::MethodNotAllowed("Method not allowed");
            }
        }
        headerParsed = true;
        if (config.CGI["status"] == "ON")
        {
            cgiMode = true;
        }
        isMultipart = header->isMultipart() && header->getMethod() == "POST";
        isChunked   = header->getIsChunked() && header->getMethod() == "POST";
        buffer.erase(buffer.begin(), buffer.begin() + header_end + 4);
        if (!isMultipart && header->getMethod() == "POST")
        {
            *isUploading = true;
            std::ostringstream oss;
            oss << "upload_" << time(NULL) << ".bin";
            filename = oss.str();
            folder += filename;
            file     = new std::ofstream(folder.c_str(), std::ios::binary);
            fileOpen = true;
        }
    }
    if (!(config.methods & 2))
    {
        if (header->getMethod() == "POST")
        {
            throw ExceptionHandler::MethodNotAllowed("Method not allowed");
        }
    }
    if (isMultipart && filename.empty() && header->isMultipart() &&
        header->getMethod() == "POST" && config.methods & 2)
    {
        *isUploading = true;
        std::string buf_str(buffer.begin(), buffer.end());
        size_t      part_header_end = buf_str.find("\r\n\r\n");
        if (part_header_end == std::string::npos)
            return;

        std::string part_header = buf_str.substr(0, part_header_end);
        std::string key         = "filename=\"";
        size_t      pos         = part_header.find(key);
        if (pos != std::string::npos)
        {
            pos += key.length();
            size_t end = part_header.find("\"", pos);
            filename   = part_header.substr(pos, end - pos);
            for (size_t i = 0; i < filename.size(); ++i)
                if (!isalnum(filename[i]) && filename[i] != '.' &&
                    filename[i] != '_')
                    filename[i] = '_';
            folder += filename;
            file     = new std::ofstream(folder.c_str(), std::ios::binary);
            fileOpen = true;
        }
        buffer.erase(buffer.begin(), buffer.begin() + part_header_end + 4);
    }

    if (fileOpen)
    {
        if (isMultipart)
        {
            // Multipart processing with boundary
            std::string boundary = "--" + header->getBoundary();
            int         bpos     = findBoundary(boundary);

            if (bpos >= 0)
            {
                if (bpos >= 2)
                    file->write(&buffer[0], bpos - 2);
                file->close();
                fileOpen = false;
                finished = true;
                buffer.erase(buffer.begin(),
                             buffer.begin() + bpos + boundary.size());
                std::cout << "File saved: " << filename << "\n";
            }
            else
            {
                if (buffer.size() > boundary.size() + 4)
                {
                    size_t write_size = buffer.size() - (boundary.size() + 4);
                    file->write(&buffer[0], write_size);
                    buffer.erase(buffer.begin(), buffer.begin() + write_size);
                }
            }
        }
        else if (isChunked)
        {
            // Dechunk the data
            std::vector<char> dechunkedData;
            size_t            bytesProcessed;

            dechunkedData = dechunkData(buffer, bytesProcessed);
            buffer.erase(buffer.begin(), buffer.begin() + bytesProcessed);

            if (!dechunkedData.empty())
            {
                if (cgiMode)
                {
                    fullRawBody.append(dechunkedData.begin(),
                                       dechunkedData.end());
                }

                // Write dechunked data to file
                file->write(&dechunkedData[0], dechunkedData.size());

                // If we've received the final chunk (chunk_size == 0)
                if (chunkedRemaining == 0 && !inChunkedData)
                {
                    file->close();
                    fileOpen = false;
                    finished = true;
                    std::cout << "File saved (chunked POST): " << filename
                              << "\n";
                }
            }
        }
        else
        {
            // Normal POST processing - write raw body to file
            size_t content_length = header->getContentLength();

            if (content_length > 0)
            {
                // Accumulate for CGI mode
                if (cgiMode)
                {
                    fullRawBody.append(buffer.begin(), buffer.end());
                }
                std::streampos bytes_written = file->tellp();
                size_t         remaining =
                    content_length - static_cast<size_t>(bytes_written);

                if (buffer.size() >= remaining)
                {
                    file->write(&buffer[0], remaining);
                    file->close();
                    fileOpen = false;
                    finished = true;
                    buffer.erase(buffer.begin(), buffer.begin() + remaining);
                    std::cout << "File saved (normal POST): " << filename
                              << "\n";
                }
                else
                {
                    file->write(&buffer[0], buffer.size());
                    buffer.clear();
                }
            }
            else
            {
                file->write(&buffer[0], buffer.size());
                buffer.clear();
                file->close();
                fileOpen = false;
                finished = true;
                std::cout << "File saved (normal POST, unknown length): "
                          << filename << "\n";
            }
        }
    }
}

Header* Upload::getHeader()
{
    return header;
}

std::string Upload::getFullRawBody() const
{
    return fullRawBody;
}