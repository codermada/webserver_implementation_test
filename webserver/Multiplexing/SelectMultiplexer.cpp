#include "SelectMultiplexer.hpp"

SelectMultiplexer::SelectMultiplexer() {}

SelectMultiplexer::SelectMultiplexer(int fd)
{
    FD_ZERO(&master_set);
    fd_max          = fd;
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
}

SelectMultiplexer::SelectMultiplexer(const SelectMultiplexer& other)
{
    *this = other;
}

SelectMultiplexer::~SelectMultiplexer() {}

SelectMultiplexer& SelectMultiplexer::operator=(const SelectMultiplexer& other)
{
    if (this != &other)
    {
        this->master_set = other.master_set;
        this->read_set = other.read_set;
        this->write_set = other.write_set;
        this->fd_max = other.fd_max;
        this->timeout = other.timeout;
    }
    return *this;
}

SelectMultiplexer::SelectMultiplexerException::SelectMultiplexerException(
    const std::string& msg)
    : message("SelectMultiplexerException: " + msg)
{
}

const char* SelectMultiplexer::SelectMultiplexerException::what() const throw()
{
    return (message.c_str());
}

SelectMultiplexer::SelectMultiplexerException::
    ~SelectMultiplexerException() throw()
{
}

void SelectMultiplexer::wait()
{
    read_set   = master_set;
    write_set  = master_set;
    int status = select(fd_max + 1, &read_set, &write_set, NULL, &timeout);
    if (status < 0)
        throw SelectMultiplexerException("select");
}

void SelectMultiplexer::registerFd(int fd)
{
    FD_SET(fd, &master_set);
    if (fd > fd_max)
        fd_max = fd;
}

void SelectMultiplexer::unregisterFd(int fd)
{
    FD_CLR(fd, &master_set);
}

int SelectMultiplexer::getFdMax()
{
    return (fd_max);
}

void SelectMultiplexer::setFdMax(int value)
{
    fd_max = value;
}

bool SelectMultiplexer::isReadyForRead(int fd)
{
    return (FD_ISSET(fd, &read_set));
}

bool SelectMultiplexer::isReadyForWrite(int fd)
{
    return (FD_ISSET(fd, &write_set));
}