#include "Webserv.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

volatile sig_atomic_t stop = 0;

static void handle_sigint(int signum)
{
    static_cast<void>(signum);
    stop = 1;
}

Webserv::Webserv() {}

Webserv::Webserv(std::string fileName)
{
    Config tmp = Config(fileName);
    configs    = tmp.getConfigs();
    n          = tmp.getN();
    tmp.printInfo();
}

Webserv::Webserv(const Webserv& other)
{
    (void)other;
}

Webserv::~Webserv()
{
    for (std::map<int, Upload*>::iterator it = uploads.begin();
         it != uploads.end(); ++it)
    {
        delete it->second;
        close(it->first);
    }
}

Webserv& Webserv::operator=(const Webserv& other)
{
    if (this != &other)
    {
        (void)other;
    }
    return *this;
}

int Webserv::getConfigId(int clientFd, std::map<int, int> serverFdToConfigId,
                         std::map<int, int> clientFdToServerFd)
{
    return (serverFdToConfigId[clientFdToServerFd[clientFd]]);
}

void Webserv::initSockets(void)
{
    Signal::set(SIGINT, handle_sigint);

    for (int i = 0; i < n; i++)
    {
        serverSockets.push_back(SocketOperation::createSocket());
    }
    for (int i = 0; i < n; i++)
    {
        SocketOperation::set_non_blocking(serverSockets[i]);
        serverFdToConfigId[serverSockets[i]] = i;
    }
    for (int i = 0; i < n; i++)
    {
        SocketOperation::setsockopt_(serverSockets[i]);
    }
    for (int i = 0; i < n; i++)
    {
        struct sockaddr_in addr =
            Address(configs[i].host, configs[i].port_str).getAddr();
        SocketOperation::bind_(serverSockets[i], addr);
    }
    for (int i = 0; i < n; i++)
    {
        SocketOperation::listen_(serverSockets[i], SOMAXCONN);
    }
}

SelectMultiplexer Webserv::initMonitor(void)
{
    int fd_max = -1;
    for (int i = 0; i < n; i++)
    {
        if (serverSockets[i] > fd_max)
            fd_max = serverSockets[i];
    }
    SelectMultiplexer monitor(fd_max);
    for (int i = 0; i < n; i++)
    {
        monitor.registerFd(serverSockets[i]);
    }
    return (monitor);
}

std::string Webserv::getFileExtension(const std::string& path)
{
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos)
    {
        return path.substr(dotPos);
    }
    return "";
}

std::string Webserv::handleCGI(const UnitConf_t&  config,
                               const std::string& method,
                               const std::string& path,
                               const std::string& queryString,
                               const std::string& body)
{
    std::string extension = getFileExtension(path);
    bool        isCGI     = (config.CGI.find(extension) != config.CGI.end());

    std::cout << "\n=== CGI REQUEST ===" << std::endl;
    std::cout << "Method: " << method << std::endl;
    std::cout << "Path: " << path << std::endl;
    std::cout << "Extension: " << extension << std::endl;
    std::cout << "Body size: " << body.size() << std::endl;

    if (!isCGI)
    {
        return "";
    }

    std::string fullPath = std::string(SUPERROOT) + "/" + config.root + path;
    std::cout << "Full script path: " << fullPath << std::endl;

    // Check if file exists
    struct stat st;
    if (stat(fullPath.c_str(), &st) != 0)
    {
        throw ExceptionHandler::NotFound("CGI script not found: " + fullPath);
    }

    // Build environment variables
    std::map<std::string, std::string> envVars;
    envVars["REQUEST_METHOD"]    = method;
    envVars["QUERY_STRING"]      = queryString;
    envVars["SCRIPT_FILENAME"]   = fullPath;
    envVars["SCRIPT_NAME"]       = path;
    envVars["DOCUMENT_ROOT"]     = std::string(SUPERROOT) + "/" + config.root;
    envVars["REDIRECT_STATUS"]   = "200";
    envVars["GATEWAY_INTERFACE"] = "CGI/1.1";
    envVars["SERVER_PROTOCOL"]   = "HTTP/1.1";
    envVars["SERVER_SOFTWARE"]   = "Webserver/1.0";
    envVars["REDIRECT_URL"]      = path;
    envVars["REDIRECT_REQUEST_METHOD"]  = method;
    envVars["REDIRECT_QUERY_STRING"]    = queryString;
    envVars["REDIRECT_REDIRECT_STATUS"] = "200";

    std::ostringstream oss;
    oss << body.size();
    envVars["CONTENT_LENGTH"] = oss.str();

    if (!body.empty())
    {
        envVars["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
    }

    // Execute CGI
    CGI         cgi;
    std::string output = cgi.execute(fullPath, body, envVars);

    // Build response string
    std::string response;
    if (output.find("HTTP/") == 0)
    {
        response = output;
    }
    else if (output.find("Content-Type") == 0)
    {
        response = "HTTP/1.1 200 OK\r\n" + output;
    }
    else
    {
        response =
            "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + output;
    }
    return response;
}

void Webserv::run(void)
{
    initSockets();
    SelectMultiplexer monitor = initMonitor();
    while (!stop)
    {
        monitor.wait();
        for (int i = 0; i <= monitor.getFdMax(); i++)
        {
            if (!monitor.isReadyForRead(i))
                continue;
            if (Validator::isInVect(i, serverSockets) == 1)
            {
                if (monitor.isReadyForRead(i))
                {
                    clientData_t client = SocketOperation::accept_(i);
                    SocketOperation::set_non_blocking(client.fd);
                    monitor.registerFd(client.fd);
                    clientFdToServerFd[client.fd] = i;

                    uploads[client.fd]        = new Upload(client.fd);
                    handlingUpload[client.fd] = 0;

                    clientFdToLastActivity[client.fd] = time(NULL);
                }
            }
            else
            {
                int         n        = 0;
                std::string body     = "";
                std::string response = "";
                UnitConf_t  config = configs[getConfigId(i, serverFdToConfigId,
                                                         clientFdToServerFd)];

                char        buffer[1024] = "";
                std::string path = SUPERROOT + std::string("/") + config.root;
                try
                {
                    if (monitor.isReadyForRead(i))
                    {
                        n = recv(i, buffer, sizeof(buffer), 0);
                        if (n <= 0)
                        {
                            delete uploads[i];
                            close(i);
                            monitor.unregisterFd(i);
                            uploads.erase(i);
                            std::cout << "Client disconnected: " << i << "\n";
                            continue;
                        }

                        uploads[i]->appendData(buffer, n);
                        std::string folder = SUPERROOT + std::string("/") +
                                             config.root + std::string("/") +
                                             config.uploads + std::string("/");
                        uploads[i]->process(config, folder, &handlingUpload[i]);

                        if (uploads[i]->isFinished() && handlingUpload[i])
                        {
                            Header      h = *(uploads[i]->getHeader());
                            std::string extension =
                                getFileExtension(h.getPath());
                            bool isCGI = (config.CGI.find(extension) !=
                                          config.CGI.end());

                            // Handle CGI POST request
                            if (isCGI && config.CGI["status"] == "ON")
                            {
                                response =
                                    handleCGI(config, h.getMethod(),
                                              h.getPath(), h.getQueryString(),
                                              uploads[i]->getFullRawBody());
                                if (monitor.isReadyForWrite(i))
                                {
                                    if (send(i, response.c_str(),
                                             response.size(), 0) > 0)
                                    {
                                        delete uploads[i];
                                        close(i);
                                        monitor.unregisterFd(i);
                                        uploads.erase(i);
                                        handlingUpload[i] = false;
                                        continue;
                                    }
                                }
                                continue;
                            }

                            // Regular upload handling (non-CGI)
                            if (config.redir.find(h.getPath()) !=
                                    config.redir.end() &&
                                config.redir["status"] == "ON")
                            {
                                std::string target = config.redir[h.getPath()];
                                if (target == h.getPath())
                                {
                                    throw ExceptionHandler::InternalServerError(
                                        "Redirect loop detected");
                                }
                                std::string redirFallback =
                                    "<html><body>"
                                    "<h1>Redirecting...</h1>"
                                    "<p>If you are not redirected "
                                    "automatically, "
                                    "<a href=\"/new-page.html\">click "
                                    "here</a>.</p>"
                                    "</body></html>";
                                response = HttpRedirection::basic(
                                    redirFallback, config.redir[h.getPath()]);
                            }
                            else
                            {
                                response = HttpSuccess::basic(
                                    "Upload ok", "Not_a_file.html");
                            }
                            if (monitor.isReadyForWrite(i))
                            {
                                if (send(i, response.c_str(), response.size(),
                                         0) > 0)
                                {
                                    delete uploads[i];
                                    close(i);
                                    monitor.unregisterFd(i);
                                    uploads.erase(i);
                                    handlingUpload[i] = false;
                                    continue;
                                }
                            }
                        }
                        if (!handlingUpload[i])
                        {
                            Header      h(buffer, config.maxBodySize);
                            std::string extension =
                                getFileExtension(h.getPath());
                            bool isCGI = (config.CGI.find(extension) !=
                                          config.CGI.end());

                            if (h.getMethod() == "GET" && config.methods & GET)
                            {
                                // Handle CGI GET request
                                if (isCGI && config.CGI["status"] == "ON")
                                {
                                    response = handleCGI(
                                        config, h.getMethod(), h.getPath(),
                                        h.getQueryString(), "");
                                    if (monitor.isReadyForWrite(i))
                                    {
                                        if (send(i, response.c_str(),
                                                 response.size(), 0) > 0)
                                        {
                                            cleanupClient(i, monitor);
                                        }
                                    }
                                }
                                else if (config.redir.find(h.getPath()) !=
                                             config.redir.end() &&
                                         config.redir["status"] == "ON")
                                {
                                    std::string target =
                                        config.redir[h.getPath()];
                                    if (target == h.getPath())
                                    {
                                        throw ExceptionHandler::
                                            InternalServerError(
                                                "Redirect loop detected");
                                    }
                                    std::string redirFallback =
                                        "<html><body>"
                                        "<h1>Redirecting...</h1>"
                                        "<p>If you are not redirected "
                                        "automatically, "
                                        "<a href=\"/new-page.html\">click "
                                        "here</a>.</p>"
                                        "</body></html>";
                                    response = HttpRedirection::basic(
                                        redirFallback,
                                        config.redir[h.getPath()]);
                                }
                                else
                                {
                                    if (!config.directoryListing)
                                        StaticFile::process(config, path, h,
                                                            body);
                                    else
                                        DirectoryListing::process(config, path,
                                                                  h, body);
                                    response =
                                        HttpSuccess::basic(body, h.getPath());
                                }
                                if (monitor.isReadyForWrite(i))
                                {
                                    if (send(i, response.c_str(),
                                             response.length(), 0) > 0)
                                    {
                                        cleanupClient(i, monitor);
                                    }
                                }
                            }
                            else if (h.getMethod() == "DELETE" &&
                                     config.methods & DELETE &&
                                     config.CGI["status"] == "OFF")
                            {
                                std::cout << h.getMethod() << " ";
                                std::string p = path + "/" + config.uploads +
                                                "/" + h.getPath();
                                std::string err;
                                if (!Validator::isFile(p, err))
                                {
                                    throw ExceptionHandler::NotFound(
                                        "File to delete not found");
                                }
                                else if (!Validator::isDirectory(p))
                                {
                                    std::ofstream out(p.c_str(),
                                                      std::ios::out |
                                                          std::ios::trunc |
                                                          std::ios::binary);
                                    if (!out)
                                    {
                                        throw ExceptionHandler::
                                            InternalServerError(
                                                "File content deletion failed");
                                    }
                                    out.close();
                                    std::cout << "File content deleted (" << p
                                              << ")" << std::endl;
                                    response = HttpSuccess::basic(
                                        std::string("File content deleted") +
                                            "(" + p + ")",
                                        "Not_a_file.html");
                                    if (monitor.isReadyForWrite(i))
                                    {
                                        if (send(i, response.c_str(),
                                                 response.length(), 0) > 0)
                                        {
                                            cleanupClient(i, monitor);
                                        }
                                    }
                                }
                                else
                                {
                                    throw ExceptionHandler::MethodNotAllowed(
                                        "Method not allowed");
                                }
                            }
                        }
                    }
                }
                catch (const ExceptionHandler::BadRequest& e)
                {
                    std::cout << e.what() << std::endl;
                    if (monitor.isReadyForWrite(i))
                    {
                        response =
                            HttpError::error(400, path + config.error[400]);
                        if (send(i, response.c_str(), response.length(), 0) > 0)
                        {
                            cleanupClient(i, monitor);
                        }
                    }
                }
                catch (const ExceptionHandler::Unauthorized& e)
                {
                    std::cout << e.what() << std::endl;
                    if (monitor.isReadyForWrite(i))
                    {
                        response =
                            HttpError::error(401, path + config.error[401]);
                        if (send(i, response.c_str(), response.length(), 0) > 0)
                        {
                            cleanupClient(i, monitor);
                        }
                    }
                }
                catch (const ExceptionHandler::Forbidden& e)
                {
                    std::cout << e.what() << std::endl;
                    if (monitor.isReadyForWrite(i))
                    {
                        response =
                            HttpError::error(403, path + config.error[403]);
                        if (send(i, response.c_str(), response.length(), 0) > 0)
                        {
                            cleanupClient(i, monitor);
                        }
                    }
                }
                catch (const ExceptionHandler::NotFound& e)
                {
                    std::cout << e.what() << std::endl;
                    if (monitor.isReadyForWrite(i))
                    {
                        response =
                            HttpError::error(404, path + config.error[404]);
                        if (send(i, response.c_str(), response.length(), 0) > 0)
                        {
                            cleanupClient(i, monitor);
                        }
                    }
                }
                catch (const ExceptionHandler::MethodNotAllowed& e)
                {
                    std::cout << e.what() << std::endl;
                    if (monitor.isReadyForWrite(i))
                    {
                        response =
                            HttpError::error(405, path + config.error[405]);
                        if (send(i, response.c_str(), response.length(), 0) > 0)
                        {
                            cleanupClient(i, monitor);
                        }
                    }
                }
                catch (const ExceptionHandler::PayloadTooLarge& e)
                {
                    std::cout << e.what() << std::endl;
                    if (monitor.isReadyForWrite(i))
                    {
                        response =
                            HttpError::error(413, path + config.error[413]);
                        if (send(i, response.c_str(), response.length(), 0) > 0)
                        {
                            cleanupClient(i, monitor);
                        }
                    }
                }
                catch (const ExceptionHandler::InternalServerError& e)
                {
                    std::cout << e.what() << std::endl;
                    if (monitor.isReadyForWrite(i))
                    {
                        response =
                            HttpError::error(500, path + config.error[500]);
                        if (send(i, response.c_str(), response.length(), 0) > 0)
                        {
                            cleanupClient(i, monitor);
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    std::cout << e.what() << std::endl;
                    if (monitor.isReadyForWrite(i))
                    {
                        response =
                            HttpError::error(500, path + config.error[500]);
                        if (send(i, response.c_str(), response.length(), 0) > 0)
                        {
                            cleanupClient(i, monitor);
                        }
                    }
                }
            }
        }
        // http request timeout ->
        for (int i = 0; i <= monitor.getFdMax(); i++)
        {
            UnitConf_t config =
                configs[getConfigId(i, serverFdToConfigId, clientFdToServerFd)];
            std::string path = SUPERROOT + std::string("/") + config.root;
            if (Validator::isInVect(i, serverSockets) == 0)
            {
                if (monitor.isReadyForRead(i) || monitor.isReadyForWrite(i))
                {
                    time_t now = time(NULL);
                    if (now - clientFdToLastActivity[i] > CLIENT_TIMEOUT)
                    {
                        if (monitor.isReadyForWrite(i))
                        {
                            std::string response =
                                HttpError::error(504, path + config.error[504]);
                            if (send(i, response.c_str(), response.length(),
                                     0) > 0)
                            {
                                cleanupClient(i, monitor);
                            }
                        }
                    }
                }
            }
        }
    }
}

void Webserv::cleanupClient(int fd, SelectMultiplexer& monitor)
{
    std::map<int, Upload*>::iterator it = uploads.find(fd);
    if (it != uploads.end())
    {
        delete it->second;
        uploads.erase(it);
    }
    close(fd);
    monitor.unregisterFd(fd);
    clientFdToServerFd.erase(fd);
    handlingUpload.erase(fd);
}