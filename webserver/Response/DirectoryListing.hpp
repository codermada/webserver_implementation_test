#ifndef DIRECTORYLISTING_HPP
#define DIRECTORYLISTING_HPP

#include "Config.hpp"
#include "ExceptionHandler.hpp"
#include "Header.hpp"
#include "File.hpp"
#include "Validator.hpp"

#include <string>

#include <dirent.h>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

class DirectoryListing
{
public:
    DirectoryListing();
    DirectoryListing(const DirectoryListing& other);
    ~DirectoryListing();

    DirectoryListing& operator=(const DirectoryListing& other);

    static std::string getListDirectory(const std::string& host, int port,
                             const std::string& path,
                             const std::string& pathName);
    static void process(UnitConf_t& config, std::string path, Header& header, std::string& body);
};

#endif
