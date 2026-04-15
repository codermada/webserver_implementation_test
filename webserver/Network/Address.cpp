#include "Address.hpp"

Address::Address() {}

Address::Address(std::string host, std::string port)
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0)
        throw AddressException("getaddrinfo");
    memcpy(&addr, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
}

Address::Address(const Address& other)
{
    (void)other;
}

Address::~Address() {}

Address& Address::operator=(const Address& other)
{
    if (this != &other)
    {
        // copy members here
        (void)other;
    }
    return *this;
}

Address::AddressException::AddressException(const std::string& msg)
    : message("AddressException: " + msg)
{
}

const char* Address::AddressException::what() const throw()
{
    return (message.c_str());
}

Address::AddressException::~AddressException() throw() {}

struct sockaddr_in& Address::getAddr()
{
    return (addr);
}
