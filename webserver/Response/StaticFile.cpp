#include "StaticFile.hpp"

StaticFile::StaticFile() {}

StaticFile::StaticFile(const StaticFile& other)
{
    (void)other;
}

StaticFile::~StaticFile() {}

StaticFile& StaticFile::operator=(const StaticFile& other)
{
    if (this != &other)
    {
        // copy members here
        (void)other;
    }
    return *this;
}

void StaticFile::process(UnitConf_t& config, std::string path, Header& header,
                         std::string& body)
{
    path += header.getPath();
    std::cout << header.getMethod() << " ";
    std::cout << path << std::endl;
    if (Validator::isDirectory(path) && config.index != "-")
    {
        try
        {
            body = File::getFileContent(path + "/" + config.index);
        }
        catch (const std::exception& e)
        {
            throw ExceptionHandler::NotFound("");
        }
    }
    else
    {
        try
        {
            body = File::getFileContent(path);
        }
        catch (const std::exception& e)
        {
            throw ExceptionHandler::NotFound("");
        }
    }
}
