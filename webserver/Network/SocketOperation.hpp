#ifndef SOCKETOPERATION_HPP
#define SOCKETOPERATION_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <fcntl.h>
#include <string>
#include <exception>


typedef struct clientData_s
{
    int fd;
    struct sockaddr_in addr;
    
} clientData_t;

class SocketOperation
{
public:
    SocketOperation();
    SocketOperation(const SocketOperation& other);
    ~SocketOperation();

    class SocketOperationException : public std::exception
    {
      private:
        std::string message;

      public:
        SocketOperationException(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~SocketOperationException() throw();
    };

    SocketOperation& operator=(const SocketOperation& other);
    static int createSocket();
    static void setsockopt_(int fd);
    static void set_non_blocking(int fd);
    static void connect_(int fd, struct sockaddr_in addr);
    static void bind_(int fd, struct sockaddr_in addr);
    static void listen_(int fd, int backlog);
    static clientData_t accept_(int fd);


};

#endif
