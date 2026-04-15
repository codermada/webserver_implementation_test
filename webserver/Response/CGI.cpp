#include "CGI.hpp"
#include "ExceptionHandler.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>

CGI::CGI() : pid(-1), finished(false), inputWritten(false)
{
    pipeIn[0]  = -1;
    pipeIn[1]  = -1;
    pipeOut[0] = -1;
    pipeOut[1] = -1;
}

CGI::CGI(const CGI& other)
{
    (void)other;
}

CGI::~CGI()
{
    if (pipeIn[0] != -1)
        close(pipeIn[0]);
    if (pipeIn[1] != -1)
        close(pipeIn[1]);
    if (pipeOut[0] != -1)
        close(pipeOut[0]);
    if (pipeOut[1] != -1)
        close(pipeOut[1]);
    if (pid > 0)
        waitpid(pid, NULL, 0);
}

CGI& CGI::operator=(const CGI& other)
{
    if (this != &other)
    {
        (void)other;
    }
    return *this;
}

std::string CGI::execute(const std::string& scriptPath, const std::string& body,
                         const std::map<std::string, std::string>& envVars)
{
    std::cout << "\n=== CGI EXECUTE ===" << std::endl;
    std::cout << "Script: " << scriptPath << std::endl;
    std::cout << "Body size: " << body.size() << std::endl;

    // Check if script exists and is readable
    if (access(scriptPath.c_str(), F_OK) != 0)
    {
        throw ExceptionHandler::NotFound("CGI script not found: " + scriptPath);
    }

    if (access(scriptPath.c_str(), R_OK) != 0)
    {
        throw ExceptionHandler::Forbidden("CGI script not readable: " +
                                          scriptPath);
    }

    // Path to php
    std::string php = "/usr/bin/php";

    if (access(php.c_str(), X_OK) != 0)
    {
        throw ExceptionHandler::InternalServerError(
            "PHP interpreter not executable: " + php);
    }

    // Create pipes
    if (pipe(pipeIn) == -1)
    {
        throw ExceptionHandler::InternalServerError(
            "Failed to create input pipe for CGI");
    }

    if (pipe(pipeOut) == -1)
    {
        close(pipeIn[0]);
        close(pipeIn[1]);
        throw ExceptionHandler::InternalServerError(
            "Failed to create output pipe for CGI");
    }

    pid = fork();

    if (pid == -1)
    {
        close(pipeIn[0]);
        close(pipeIn[1]);
        close(pipeOut[0]);
        close(pipeOut[1]);
        throw ExceptionHandler::InternalServerError(
            "Failed to fork for CGI process");
    }

    if (pid == 0)
    {
        // Child process
        dup2(pipeIn[0], STDIN_FILENO);
        dup2(pipeOut[1], STDOUT_FILENO);

        close(pipeIn[0]);
        close(pipeIn[1]);
        close(pipeOut[0]);
        close(pipeOut[1]);

        // Build environment variables
        int    envCount   = envVars.size();
        char** envp       = new char*[envCount + 1];
        char** envStrings = new char*[envCount];

        int idx = 0;
        for (std::map<std::string, std::string>::const_iterator it =
                 envVars.begin();
             it != envVars.end(); ++it)
        {
            std::string envStr = it->first + "=" + it->second;
            envStrings[idx]    = new char[envStr.size() + 1];
            strcpy(envStrings[idx], envStr.c_str());
            envp[idx] = envStrings[idx];
            idx++;
        }
        envp[envCount] = NULL;

        // Execute PHP
        char* args[3];
        args[0] = const_cast<char*>(php.c_str());
        args[1] = const_cast<char*>(scriptPath.c_str());
        args[2] = NULL;

        execve(args[0], args, envp);

        // Cleanup on failure
        for (int i = 0; i < envCount; i++)
        {
            delete[] envStrings[i];
        }
        delete[] envStrings;
        delete[] envp;

        std::cerr << "execve failed: " << strerror(errno) << std::endl;
        exit(1);
    }

    // Parent process
    close(pipeIn[0]);
    close(pipeOut[1]);

    // fcntl(pipeIn[1], F_SETFL, O_NONBLOCK);
    // fcntl(pipeOut[0], F_SETFL, O_NONBLOCK);
    // Write body to CGI - ensure all bytes are written
    if (!body.empty())
    {
        std::cout << "Writing " << body.size() << " bytes to CGI" << std::endl;
        ssize_t     totalWritten = 0;
        const char* data         = body.c_str();
        size_t      remaining    = body.size();

        while (remaining > 0)
        {
            ssize_t written = write(pipeIn[1], data + totalWritten, remaining);
            if (written > 0)
            {
                totalWritten += written;
                remaining -= written;
                std::cout << "Wrote " << written
                          << " bytes, total: " << totalWritten << std::endl;
            }
            else
            {
                std::cerr << "Write failed: " << strerror(errno) << std::endl;
                break;
            }
        }
    }

    // IMPORTANT: Close input pipe to signal EOF to PHP
    close(pipeIn[1]);
    std::cout << "Closed input pipe (EOF sent to CGI)" << std::endl;

    // Read output from CGI
    std::string output;
    char        buffer[4096];
    int         n;
    while ((n = read(pipeOut[0], buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[n] = '\0';
        output.append(buffer, n);
        std::cout << "Read " << n << " bytes from CGI" << std::endl;
    }
    close(pipeOut[0]);

    // Wait for child process
    int status;
    waitpid(pid, &status, 0);

    std::cout << "Total output size: " << output.size() << " bytes"
              << std::endl;

    return output;
}
