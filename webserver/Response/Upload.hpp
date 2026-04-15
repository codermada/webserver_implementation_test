#ifndef UPLOAD_HPP
#define UPLOAD_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sys/socket.h>
#include "Header.hpp"
#include "Config.hpp"
#include "HttpRedirection.hpp"
#include "SelectMultiplexer.hpp"

class Upload {
private:
    int fd;
    std::vector<char> buffer;
    bool headerParsed;
    Header* header;
    std::ofstream* file;
    std::string filename;
    bool fileOpen;
    bool finished;
    bool isMultipart;
    bool isChunked;
    size_t chunkedRemaining;
    bool inChunkedData;
    bool cgiMode;
    std::string fullRawBody;

    int findBoundary(const std::string& boundary);

public:
    Upload(int client_fd);
    ~Upload();

    int getFd() const;
    bool isFinished() const;

    std::vector<char> dechunkData(const std::vector<char>& data, size_t& bytesProcessed);
    void appendData(const char* data, size_t size);
    void process(UnitConf_t config, std::string folder, bool *isUploading);
    Header* getHeader();
    std::string getFullRawBody() const;
};

#endif
