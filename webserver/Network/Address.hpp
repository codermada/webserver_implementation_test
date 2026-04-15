#ifndef ADDRESS_HPP
#define ADDRESS_HPP

#include <cstring>
#include <netinet/in.h>
#include <string>
#include <netdb.h>

class Address
{
private:
    struct sockaddr_in addr;
    
public:
    Address();
    Address(std::string host, std::string port);
    Address(const Address& other);
    ~Address();

    Address& operator=(const Address& other);

    class AddressException : public std::exception
    {
      private:
        std::string message;

      public:
        AddressException(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~AddressException() throw();
    };

    struct sockaddr_in &getAddr();

};

#endif
