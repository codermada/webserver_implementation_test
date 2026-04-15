#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <map>
#include <vector>
#include <fcntl.h>

class CGI
{
public:
    CGI();
    CGI(const CGI& other);
    ~CGI();

    CGI& operator=(const CGI& other);
    
    std::string execute(const std::string& scriptPath, 
                        const std::string& body,
                        const std::map<std::string, std::string>& envVars);
    
private:
    int pipeIn[2];
    int pipeOut[2];
    int pid;
    std::string output;
    std::string inputBuffer;
    bool finished;
    bool inputWritten;
};

#endif