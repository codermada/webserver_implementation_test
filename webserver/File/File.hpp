#ifndef FILE_HPP
#define FILE_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <exception>

class File
{
public:
    File();
    File(const File& other);
    ~File();

    File& operator=(const File& other);


    static std::string getFileContent(std::string fileName);
};

#endif
