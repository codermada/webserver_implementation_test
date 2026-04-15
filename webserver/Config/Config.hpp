#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Validator.hpp"
#include "File.hpp"
#include "helpers.hpp"

#include <exception>
#include <map>
#include <sstream>
#include <vector>

enum HttpMethod
{
    GET    = 1, // 001
    POST   = 2, // 010
    DELETE = 4  // 100
};

typedef struct UnitConf
{
    std::string host;
    int         port;
    std::string port_str;
    int         methods;
    int        directoryListing;
    std::string root;
    std::string uploads;
    std::string index;
    int maxBodySize;
    std::map<std::string, std::string> redir;
    std::map<std::string, std::string> CGI;
    std::map<int, std::string> error;
} UnitConf_t;

class Config
{
  private:
    std::string              fileContent;
    int                      n;
    std::vector<std::string> serverBlocks;
    std::vector<UnitConf_t>  configs;

  public:
    Config();
    Config(std::string filename);
    Config(const Config& other);
    ~Config();

    Config& operator=(const Config& other);

    class ConfigException : public std::exception
    {
      private:
        std::string message;

      public:
        ConfigException(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~ConfigException() throw();
    };

    void parseFileContent();
    void parseServerBlock(std::string serverBlock, int j);

    void checkPorts();
    void checkDirectoryListing();
    void checkCGI();


    std::vector<UnitConf_t> getConfigs() const;
    int                     getN() const;
    void printInfo();
};

#endif
