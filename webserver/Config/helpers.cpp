#include "helpers.hpp"

int countWords(const std::string &str)
{
    std::istringstream iss(str);
    std::string word;
    int count = 0;

    while (iss >> word)
        count++;

    return count;
}

void fillMap(std::map<std::string, std::string> &m, std::string s)
{
    if (m.empty())
        return;
    for (std::map<std::string, std::string>::iterator it = m.begin(); it != m.end(); ++it)
    {
        it->second = s;
    }
}


void fillMap(std::map<int, std::string> &m, std::string s, size_t startIndex)
{
    if (m.empty())
        return;
    std::map<int, std::string>::iterator it = m.begin();
    size_t i = 0;
    while (it != m.end() && i < startIndex)
    {
        ++it;
        ++i;
    }
    for (; it != m.end(); ++it)
    {
        it->second = s;
    }
}

void printMap(const std::map<int, std::string> &m)
{
    for (std::map<int, std::string>::const_iterator it = m.begin();
         it != m.end(); ++it)
    {
        std::cout << it->first << " => " << it->second << std::endl;
    }
}