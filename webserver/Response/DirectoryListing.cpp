#include "DirectoryListing.hpp"

DirectoryListing::DirectoryListing() {}

DirectoryListing::DirectoryListing(const DirectoryListing& other)
{
    (void)other;
}

DirectoryListing::~DirectoryListing() {}

DirectoryListing& DirectoryListing::operator=(const DirectoryListing& other)
{
    if (this != &other)
    {
        // copy members here
        (void)other;
    }
    return *this;
}

std::string DirectoryListing::getListDirectory(const std::string& host,
                                               int                port,
                                               const std::string& path,
                                               const std::string& pathName)
{
    if (path.find("..") != std::string::npos)
    {
        return ("Error");
    }

    DIR* dir = opendir(path.c_str());
    if (!dir)
        return ("Cannot open directory");
    std::stringstream html;
    html << "<html>\n<body>\n";
    html << "<h6>Directory listing for " << pathName << "</h6>\n<hr>\n<ul>\n";
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        if (Validator::isDirectory(path + "/" + std::string(entry->d_name)))
            name += "/";
        if (name == "./" || name == "../")
            continue;
        html << "<li><a href=\"http://" << host << ":" << port << pathName
             << "/" << name << "\">" << name << "</a></li>\n";
    }
    html << "</ul>\n<hr>\n</body>\n</html>\n";
    closedir(dir);
    return (html.str());
}

void DirectoryListing::process(UnitConf_t& config, std::string path,
                               Header& header, std::string& body)
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
            body = getListDirectory(config.host, config.port, path,
                                    header.getPath());
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