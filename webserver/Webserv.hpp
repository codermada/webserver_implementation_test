#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Config.hpp"
#include "Signal.hpp"
#include "SocketOperation.hpp"
#include "Address.hpp"
#include "SelectMultiplexer.hpp"
#include "HttpError.hpp"
#include "HttpRedirection.hpp"
#include "HttpSuccess.hpp"
#include "Header.hpp"
#include "StaticFile.hpp"
#include "DirectoryListing.hpp"
#include "Upload.hpp"
#include "CGI.hpp"



#include <string>
#include <iostream>

// http request timeout ->
#include <ctime>
#define CLIENT_TIMEOUT 10 // seconds
// <- http request timeout

class Webserv
{
private:
    std::vector<UnitConf_t> configs;
    int n;

    std::vector<int> serverSockets;

    std::map<int, int> serverFdToConfigId;
    std::map<int, int> clientFdToServerFd;

    std::map<int, Upload*> uploads;
    std::map<int, bool> handlingUpload;

    // http request timeout ->
    std::map<int, time_t> clientFdToLastActivity;
    // <- http request timeout

public:
    Webserv();
    Webserv(std::string fileName);
    Webserv(const Webserv& other);
    ~Webserv();

    Webserv& operator=(const Webserv& other);

    int getConfigId(int clientFd, std::map<int, int> serverFdToConfigId,
                       std::map<int, int> clientFdToServerFd);
    void initSockets(void);
    SelectMultiplexer initMonitor(void);
    std::string getFileExtension(const std::string& path);
    std::string handleCGI(const UnitConf_t& config, 
                             const std::string& method, const std::string& path,
                             const std::string& queryString, const std::string& body);
    void run(void);
    void cleanupClient(int fd, SelectMultiplexer &monitor);
};

#endif
