#include "Config.hpp"

Config::Config() {}

Config::Config(std::string filename)
{
    n           = 0;
    fileContent = File::getFileContent(filename);
    parseFileContent();
}

Config::Config(const Config& other)
{
    this->fileContent  = other.fileContent;
    this->n            = other.n;
    this->serverBlocks = other.serverBlocks;
    this->configs      = other.configs;
}

Config::~Config() {}

Config& Config::operator=(const Config& other)
{
    if (this != &other)
    {
        this->fileContent  = other.fileContent;
        this->n            = other.n;
        this->serverBlocks = other.serverBlocks;
        this->configs      = other.configs;
    }
    return *this;
}

Config::ConfigException::ConfigException(const std::string& msg)
    : message("ConfigException: " + msg)
{
}

const char* Config::ConfigException::what() const throw()
{
    return (message.c_str());
}

Config::ConfigException::~ConfigException() throw() {}

void Config::parseFileContent()
{
    int                i = -1;
    std::istringstream iss(fileContent);
    std::string        line;

    while (std::getline(iss, line))
    {
        // ignore comments
        if (line.size() != 0)
        {
            if (line.at(0) == '#')
            {
                continue;
            }
        }
        if (line == "server:")
        {
            i++;
            serverBlocks.push_back("");
            continue;
        }
        if (i >= 0)
        {
            serverBlocks[i] += line + '\n';
        }
    }
    if (i == -1)
        throw ConfigException("No 'server:' keyword");
    n = i + 1;
    for (int j = 0; j < n; j++)
    {
        UnitConf_t u;
        u.host             = "-";
        u.port             = -1;
        u.methods          = 0;
        u.directoryListing = -1;
        u.root             = "-";
        u.uploads          = "-";
        u.index            = "-";
        u.maxBodySize      = 52428800;
        u.redir["status"] = "OFF";
        u.CGI["status"] = "OFF";
        // here
        configs.push_back(u);
    }
    for (int j = 0; j < n; j++)
    {
        parseServerBlock(serverBlocks[j], j);
    }
    for (int j = 0; j < n; j++)
    {
        if (configs[j].host == "-")
            throw ConfigException("host field should be filled");
        if (configs[j].port == -1)
            throw ConfigException("port field should be filled");
        if (configs[j].methods == 0)
            throw ConfigException("method field should be filled");
        if (configs[j].directoryListing == -1)
            throw ConfigException("directoryListing field should be filled");
        if (configs[j].root == "-")
            throw ConfigException("root field should be filled");
        if (configs[j].uploads == "-")
            throw ConfigException("uploads field should be filled");
        // here
    }
    checkPorts();
    checkDirectoryListing();
    // checkCGI();
}

void Config::parseServerBlock(std::string serverBlock, int j)
{
    std::istringstream         iss(serverBlock);
    std::string                line;
    std::map<std::string, int> counts;
    counts["host"]             = 0;
    counts["port"]             = 0;
    counts["methods"]          = 0;
    counts["directoryListing"] = 0;
    counts["root"]             = 0;
    counts["uploads"]          = 0;
    counts["index"]            = 0;
    counts["maxBodySize"]      = 0;
    counts["redir"]      = 0;
    counts["CGI"]      = 0;
    counts["error"]      = 0;


    // here

    // error ->
    size_t start_index = 0;
    // <- error

    int i;
    while (std::getline(iss, line))
    {
        std::istringstream iss_(line);
        std::string        word;
        std::string        currentKey;
        // redir ->
        std::string oldPath = "";
        // <- redir
        // cgi ->
        std::string type = "";
        // cgi <-
        
        i = 0;
        int wordCount = countWords(line);
        while (std::getline(iss_, word, ' '))
        {
            if (i == 0)
            {
                if (word != "host" && word != "port" && word != "methods" &&
                    word != "directoryListing" && word != "root" &&
                    word != "uploads" && word != "index" &&
                    word != "maxBodySize" && word != "redir" && word != "CGI" && word != "error") // here
                {
                    throw ConfigException("wrong key -> '" + word + "'");
                }
                counts[word]++;
                currentKey = word;
            }
            else
            {
                if (currentKey == "host")
                {
                    if (!Validator::validateHost(word))
                        throw ConfigException("wrong value -> '" + word + "'");
                    configs[j].host = word;
                    if (i > 1)
                        throw ConfigException("host can have only one value");
                }
                if (currentKey == "port")
                {
                    if (!Validator::validatePort(word))
                        throw ConfigException("wrong value -> '" + word + "'");
                    configs[j].port     = std::atoi(word.c_str());
                    configs[j].port_str = word;
                    if (i > 1)
                        throw ConfigException("port can have only one value");
                }
                if (currentKey == "methods")
                {
                    if (!Validator::validateMethod(word))
                        throw ConfigException(
                            "method should be GET , POST or DELETE");
                    if (word == "GET")
                        configs[j].methods |= GET;
                    if (word == "POST")
                        configs[j].methods |= POST;
                    if (word == "DELETE")
                        configs[j].methods |= DELETE;
                    if (i > 3)
                        throw ConfigException(
                            "methods can have only three values");
                }
                if (currentKey == "directoryListing")
                {
                    if (!Validator::validateDirectoryListing(word))
                        throw ConfigException(
                            "directoryListing should be on/ON or off/OFF");
                    if (word == "ON" || word == "on")
                        configs[j].directoryListing = 1;
                    if (word == "off" || word == "OFF")
                        configs[j].directoryListing = 0;
                    if (i != 1)
                        throw ConfigException(
                            "directoryListing can have only one value");
                }
                if (currentKey == "root")
                {
                    if (!Validator::validateRoot(word))
                        throw ConfigException("root directory should exist");
                    configs[j].root = word;
                    if (i > 1)
                        throw ConfigException("root can have only one value");
                }
                if (currentKey == "uploads")
                {
                    if (!Validator::validateUploads(configs[j].root, word))
                        throw ConfigException("uploads directory should exist");
                    configs[j].uploads = word;
                    if (i > 1)
                        throw ConfigException(
                            "uploads can have only one value");
                }
                if (currentKey == "index")
                {
                    if (word != "-")
                    {
                        if (!Validator::validateIndex(word))
                            throw ConfigException("index should be .html .htm or .php");
                    }
                    configs[j].index = word; // if '-', no autoindexing
                    if (i != 1)
                        throw ConfigException("index can have only one value");
                }
                if (currentKey == "maxBodySize")
                {
                    if (!Validator::validateMaxBodySize(word))
                        throw ConfigException(
                            "maxBodySize should be less than 52428800");
                    configs[j].maxBodySize = std::atoi(word.c_str());
                    if (i != 1)
                        throw ConfigException(
                            "maxBodySize can have only one value");
                }
                if (currentKey == "redir")
                {
                    if (i == 1)
                    {
                        oldPath = word;
                    }
                    if (i == 2)
                    {
                        configs[j].redir[oldPath] = word;
                        oldPath = "";
                        configs[j].redir["status"] = "ON";
                    }
                    if (i > 2)
                        throw ConfigException(
                            "redir can have only 2 values (oldPath, newPath)");
                }
                if (currentKey == "CGI")
                {
                    if (i == 1)
                    {
                        type = word;
                    }
                    if (i == 2)
                    {
                        configs[j].CGI[type] = word;
                        type = "";
                        configs[j].CGI["status"] = "ON";
                    }
                    if (i > 2)
                        throw ConfigException(
                            "CGI can have only 2 values (type, extension)");
                }
                if (currentKey == "error")
                {
                    if (i <= wordCount - 2 && i > 0)
                    {
                        if (!Validator::validateErrorCode(word))
                            throw ConfigException(
                            "invalid error code");
                        int code = std::atoi(word.c_str());
                        configs[j].error[code] = "";
                    }
                    if (i == wordCount - 1  && i > 0)
                    {
                        if (!Validator::validateURI(word))
                            throw ConfigException(
                            "invalid URI -> '" + word + "'" );
                        std::cout << word << std::endl;
                        fillMap(configs[j].error, word, start_index);
                        start_index = i - 1;
                    }
                }
                // here
                // std::cout << word << std::endl;
            }
            i++;
        }
    }
    if (counts["host"] > 1)
        throw ConfigException("duplicate keys -> 'host'");
    if (counts["port"] > 1)
        throw ConfigException("duplicate keys -> 'port'");
    if (counts["methods"] > 1)
        throw ConfigException("duplicate keys -> 'methods'");
    if (counts["directoryListing"] > 1)
        throw ConfigException("duplicate keys -> 'directoryListing'");
    if (counts["root"] > 1)
        throw ConfigException("duplicate keys -> 'root'");
    if (counts["uploads"] > 1)
        throw ConfigException("duplicate keys -> 'uploads'");
    if (counts["index"] > 1)
        throw ConfigException("duplicate keys -> 'index'");
    if (counts["maxBodySize"] > 1)
        throw ConfigException("duplicate keys -> 'maxBodySize'");
    // here
}

void Config::checkPorts()
{
    std::vector<UnitConf_t> configs_cpy;
    configs_cpy = configs;
    int count;
    for (std::vector<UnitConf_t>::iterator it_cpy = configs_cpy.begin();
         it_cpy != configs_cpy.end(); it_cpy++)
    {
        count = 0;
        for (std::vector<UnitConf_t>::iterator it = configs.begin();
             it != configs.end(); it++)
        {
            if (it_cpy->port == it->port)
                count++;
        }
        if (count != 1)
            throw ConfigException("duplicate port");
    }
}

void Config::checkDirectoryListing()
{

    for (std::vector<UnitConf_t>::iterator it = configs.begin();
         it != configs.end(); it++)
    {
        if (it->directoryListing == 1)
        {
            if (it->methods != GET)
                throw ConfigException("directory listing supports only GET");
        }
    }
}

void Config::checkCGI()
{
    for (std::vector<UnitConf_t>::iterator it = configs.begin();
         it != configs.end(); it++)
    {
        if (it->CGI["status"] == "ON")
        {
            if (!(it->methods & GET || it->methods & POST ) || (it->methods & DELETE))
                throw ConfigException("CGI ON supports only GET and POST");
            if (it->directoryListing)
            {
                throw ConfigException("if directory listing ON, CGI cannot be ON");
            }
        }
    }
}

std::vector<UnitConf_t> Config::getConfigs() const
{
    return (configs);
}

int Config::getN() const
{
    return (n);
}

void Config::printInfo()
{
    for (int i = 0; i < n; i++)
    {

        std::cout << "server " << i << ":" << std::endl
                  << "    host             = " << configs[i].host << std::endl
                  << "    port             = " << configs[i].port << std::endl
                  << "    methods          = ";
        bool first = true;
        if (configs[i].methods & GET)
        {
            std::cout << "GET";
            first = false;
        }
        if (configs[i].methods & POST)
        {
            if (!first)
                std::cout << ", ";
            std::cout << "POST";
            first = false;
        }
        if (configs[i].methods & DELETE)
        {
            if (!first)
                std::cout << ", ";
            std::cout << "DELETE";
        }
        std::cout << std::endl;
        std::cout << "    directoryListing = "
                  << ((configs[i].directoryListing == 1) ? "ON" : "OFF")
                  << std::endl
                  << "    root             = " << configs[i].root << std::endl
                  << "    uploads          = " << configs[i].uploads
                  << std::endl
                  << "    index            = " << configs[i].index << std::endl
                  << "    maxBodySize      = " << configs[i].maxBodySize
                  << " bytes" << std::endl
                  << "    Redirection      = " << configs[i].redir["status"] << std::endl
                  << "    CGI              = " << configs[i].CGI["status"]
                  << std::endl;
        if (configs[i].error.size() != 0)
            std::cout << "    error ="  << std::endl;
        printMap(configs[i].error);
        std::cout << std::endl;
    }
}
