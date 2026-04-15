#ifndef EXCEPTIONHANDLER_HPP
#define EXCEPTIONHANDLER_HPP

#include <stdexcept>
#include <string>

class ExceptionHandler
{
public:
    ExceptionHandler();
    ExceptionHandler(const ExceptionHandler& other);
    ~ExceptionHandler();

    ExceptionHandler& operator=(const ExceptionHandler& other);

    // 4xx Client Errors

    class BadRequest : public std::exception
    {
    private:
        std::string message;
    public:
        BadRequest(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~BadRequest() throw();
    };

    class Unauthorized : public std::exception
    {
    private:
        std::string message;
    public:
        Unauthorized(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~Unauthorized() throw();
    };

    class PaymentRequired : public std::exception
    {
    private:
        std::string message;
    public:
        PaymentRequired(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~PaymentRequired() throw();
    };

    class Forbidden : public std::exception
    {
    private:
        std::string message;
    public:
        Forbidden(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~Forbidden() throw();
    };

    class NotFound : public std::exception
    {
    private:
        std::string message;
    public:
        NotFound(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~NotFound() throw();
    };

    class MethodNotAllowed : public std::exception
    {
    private:
        std::string message;
    public:
        MethodNotAllowed(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~MethodNotAllowed() throw();
    };

    class NotAcceptable : public std::exception
    {
    private:
        std::string message;
    public:
        NotAcceptable(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~NotAcceptable() throw();
    };

    class ProxyAuthenticationRequired : public std::exception
    {
    private:
        std::string message;
    public:
        ProxyAuthenticationRequired(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~ProxyAuthenticationRequired() throw();
    };

    class RequestTimeout : public std::exception
    {
    private:
        std::string message;
    public:
        RequestTimeout(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~RequestTimeout() throw();
    };

    class Conflict : public std::exception
    {
    private:
        std::string message;
    public:
        Conflict(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~Conflict() throw();
    };

    class Gone : public std::exception
    {
    private:
        std::string message;
    public:
        Gone(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~Gone() throw();
    };

    class LengthRequired : public std::exception
    {
    private:
        std::string message;
    public:
        LengthRequired(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~LengthRequired() throw();
    };

    class PreconditionFailed : public std::exception
    {
    private:
        std::string message;
    public:
        PreconditionFailed(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~PreconditionFailed() throw();
    };

    class PayloadTooLarge : public std::exception
    {
    private:
        std::string message;
    public:
        PayloadTooLarge(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~PayloadTooLarge() throw();
    };

    class URITooLong : public std::exception
    {
    private:
        std::string message;
    public:
        URITooLong(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~URITooLong() throw();
    };

    class UnsupportedMediaType : public std::exception
    {
    private:
        std::string message;
    public:
        UnsupportedMediaType(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~UnsupportedMediaType() throw();
    };

    class RangeNotSatisfiable : public std::exception
    {
    private:
        std::string message;
    public:
        RangeNotSatisfiable(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~RangeNotSatisfiable() throw();
    };

    class ExpectationFailed : public std::exception
    {
    private:
        std::string message;
    public:
        ExpectationFailed(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~ExpectationFailed() throw();
    };

    class ImATeapot : public std::exception
    {
    private:
        std::string message;
    public:
        ImATeapot(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~ImATeapot() throw();
    };

    class UpgradeRequired : public std::exception
    {
    private:
        std::string message;
    public:
        UpgradeRequired(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~UpgradeRequired() throw();
    };

    // 5xx Server Errors

    class InternalServerError : public std::exception
    {
    private:
        std::string message;
    public:
        InternalServerError(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~InternalServerError() throw();
    };

    class NotImplemented : public std::exception
    {
    private:
        std::string message;
    public:
        NotImplemented(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~NotImplemented() throw();
    };

    class BadGateway : public std::exception
    {
    private:
        std::string message;
    public:
        BadGateway(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~BadGateway() throw();
    };

    class ServiceUnavailable : public std::exception
    {
    private:
        std::string message;
    public:
        ServiceUnavailable(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~ServiceUnavailable() throw();
    };

    class GatewayTimeout : public std::exception
    {
    private:
        std::string message;
    public:
        GatewayTimeout(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~GatewayTimeout() throw();
    };

    class HttpVersionNotSupported : public std::exception
    {
    private:
        std::string message;
    public:
        HttpVersionNotSupported(const std::string& msg);
        virtual const char* what() const throw();
        virtual ~HttpVersionNotSupported() throw();
    };
};

#endif