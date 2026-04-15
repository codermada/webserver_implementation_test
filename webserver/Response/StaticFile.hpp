#ifndef STATICFILE_HPP
#define STATICFILE_HPP

#include "Config.hpp"
#include "ExceptionHandler.hpp"
#include "Header.hpp"
#include "File.hpp"
#include "Validator.hpp"

#include <string>

class StaticFile
{
public:
    StaticFile();
    
    StaticFile(const StaticFile& other);
    ~StaticFile();

    StaticFile& operator=(const StaticFile& other);

    static void process(UnitConf_t& config, std::string path, Header& header, std::string& body);
};

#endif
