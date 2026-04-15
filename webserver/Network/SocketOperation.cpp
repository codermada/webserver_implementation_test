#include "SocketOperation.hpp"

SocketOperation::SocketOperation() {}

SocketOperation::SocketOperation(const SocketOperation& other)
{
    (void)other;
}

SocketOperation::~SocketOperation() {}

SocketOperation& SocketOperation::operator=(const SocketOperation& other)
{
    if (this != &other)
    {
        // copy members here
        (void)other;
    }
    return *this;
}

SocketOperation::SocketOperationException::SocketOperationException(
    const std::string& msg)
    : message("SocketOperationException: " + msg)
{
}

const char* SocketOperation::SocketOperationException::what() const throw()
{
    return (message.c_str());
}

SocketOperation::SocketOperationException::~SocketOperationException() throw()
{
}

int SocketOperation::createSocket()
{
    int res = socket(AF_INET, SOCK_STREAM, 0);
    return (res);
}

void SocketOperation::setsockopt_(int fd)
{
    int opt    = 1;
    int status = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (status < 0)
        throw SocketOperationException("setsockopt");
}

void SocketOperation::set_non_blocking(int fd)
{
    int status = fcntl(fd, F_SETFL, O_NONBLOCK);
    if (status < 0)
        throw SocketOperationException("fcntl");
}

void SocketOperation::connect_(int fd, struct sockaddr_in addr)
{
    int status =
        connect(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    if (status < 0)
        throw SocketOperationException("connect");
}

void SocketOperation::bind_(int fd, struct sockaddr_in addr)
{
    int status =
        bind(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    if (status < 0)
        throw SocketOperationException("bind");
}

void SocketOperation::listen_(int fd, int backlog)
{
    int status = listen(fd, backlog);
    if (status < 0)
        throw SocketOperationException("listen");
}

clientData_t SocketOperation::accept_(int fd)
{
    clientData_t data;
    socklen_t    addrlen = sizeof(data.addr);
    data.fd =
        accept(fd, reinterpret_cast<struct sockaddr*>(&data.addr), &addrlen);
    if (data.fd < 0)
        throw SocketOperationException("accept");
    return (data);
}
