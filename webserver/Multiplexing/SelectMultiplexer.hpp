#ifndef SELECTMULTIPLEXER_HPP
#define SELECTMULTIPLEXER_HPP

#include <cstdlib>
#include <sys/select.h>
#include <sys/time.h>

#include <string>
#include <stdexcept>

class SelectMultiplexer
{
private:
    fd_set master_set;
    fd_set read_set;
    fd_set write_set;
    int fd_max;
    struct timeval timeout;

    SelectMultiplexer();
    
public:
    SelectMultiplexer(int fd); 
    SelectMultiplexer(const SelectMultiplexer& other);
    SelectMultiplexer& operator=(const SelectMultiplexer& other);   
    ~SelectMultiplexer();

    class SelectMultiplexerException : public std::exception
    {
      private:
        std::string message;

      public:
        SelectMultiplexerException(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~SelectMultiplexerException() throw();
    };

    void wait();
    void registerFd(int fd);
    void unregisterFd(int fd);
    int getFdMax();
    void setFdMax(int value);
    bool isReadyForRead(int fd);
    bool isReadyForWrite(int fd);


};

#endif
