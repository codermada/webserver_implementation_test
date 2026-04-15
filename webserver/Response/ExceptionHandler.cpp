#include "ExceptionHandler.hpp"

ExceptionHandler::ExceptionHandler() {}
ExceptionHandler::ExceptionHandler(const ExceptionHandler& other)
{
    (void)other;
}
ExceptionHandler::~ExceptionHandler() {}

ExceptionHandler& ExceptionHandler::operator=(const ExceptionHandler& other)
{
    if (this != &other)
        (void)other;
    return *this;
}

// ===== 4xx =====

ExceptionHandler::BadRequest::BadRequest(const std::string& msg) : message(msg)
{
}
const char* ExceptionHandler::BadRequest::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::BadRequest::~BadRequest() throw() {}

ExceptionHandler::Unauthorized::Unauthorized(const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::Unauthorized::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::Unauthorized::~Unauthorized() throw() {}

ExceptionHandler::PaymentRequired::PaymentRequired(const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::PaymentRequired::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::PaymentRequired::~PaymentRequired() throw() {}

ExceptionHandler::Forbidden::Forbidden(const std::string& msg) : message(msg) {}
const char* ExceptionHandler::Forbidden::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::Forbidden::~Forbidden() throw() {}

ExceptionHandler::NotFound::NotFound(const std::string& msg) : message(msg) {}
const char* ExceptionHandler::NotFound::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::NotFound::~NotFound() throw() {}

ExceptionHandler::MethodNotAllowed::MethodNotAllowed(const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::MethodNotAllowed::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::MethodNotAllowed::~MethodNotAllowed() throw() {}

ExceptionHandler::NotAcceptable::NotAcceptable(const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::NotAcceptable::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::NotAcceptable::~NotAcceptable() throw() {}

ExceptionHandler::ProxyAuthenticationRequired::ProxyAuthenticationRequired(
    const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::ProxyAuthenticationRequired::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::ProxyAuthenticationRequired::
    ~ProxyAuthenticationRequired() throw()
{
}

ExceptionHandler::RequestTimeout::RequestTimeout(const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::RequestTimeout::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::RequestTimeout::~RequestTimeout() throw() {}

ExceptionHandler::Conflict::Conflict(const std::string& msg) : message(msg) {}
const char* ExceptionHandler::Conflict::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::Conflict::~Conflict() throw() {}

ExceptionHandler::Gone::Gone(const std::string& msg) : message(msg) {}
const char* ExceptionHandler::Gone::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::Gone::~Gone() throw() {}

ExceptionHandler::LengthRequired::LengthRequired(const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::LengthRequired::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::LengthRequired::~LengthRequired() throw() {}

ExceptionHandler::PreconditionFailed::PreconditionFailed(const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::PreconditionFailed::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::PreconditionFailed::~PreconditionFailed() throw() {}

ExceptionHandler::PayloadTooLarge::PayloadTooLarge(const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::PayloadTooLarge::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::PayloadTooLarge::~PayloadTooLarge() throw() {}

ExceptionHandler::URITooLong::URITooLong(const std::string& msg) : message(msg)
{
}
const char* ExceptionHandler::URITooLong::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::URITooLong::~URITooLong() throw() {}

ExceptionHandler::UnsupportedMediaType::UnsupportedMediaType(
    const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::UnsupportedMediaType::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::UnsupportedMediaType::~UnsupportedMediaType() throw() {}

ExceptionHandler::RangeNotSatisfiable::RangeNotSatisfiable(
    const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::RangeNotSatisfiable::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::RangeNotSatisfiable::~RangeNotSatisfiable() throw() {}

ExceptionHandler::ExpectationFailed::ExpectationFailed(const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::ExpectationFailed::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::ExpectationFailed::~ExpectationFailed() throw() {}

ExceptionHandler::ImATeapot::ImATeapot(const std::string& msg) : message(msg) {}
const char* ExceptionHandler::ImATeapot::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::ImATeapot::~ImATeapot() throw() {}

ExceptionHandler::UpgradeRequired::UpgradeRequired(const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::UpgradeRequired::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::UpgradeRequired::~UpgradeRequired() throw() {}

// ===== 5xx =====

ExceptionHandler::InternalServerError::InternalServerError(
    const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::InternalServerError::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::InternalServerError::~InternalServerError() throw() {}

ExceptionHandler::NotImplemented::NotImplemented(const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::NotImplemented::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::NotImplemented::~NotImplemented() throw() {}

ExceptionHandler::BadGateway::BadGateway(const std::string& msg) : message(msg)
{
}
const char* ExceptionHandler::BadGateway::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::BadGateway::~BadGateway() throw() {}

ExceptionHandler::ServiceUnavailable::ServiceUnavailable(const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::ServiceUnavailable::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::ServiceUnavailable::~ServiceUnavailable() throw() {}

ExceptionHandler::GatewayTimeout::GatewayTimeout(const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::GatewayTimeout::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::GatewayTimeout::~GatewayTimeout() throw() {}

ExceptionHandler::HttpVersionNotSupported::HttpVersionNotSupported(
    const std::string& msg)
    : message(msg)
{
}
const char* ExceptionHandler::HttpVersionNotSupported::what() const throw()
{
    return message.c_str();
}
ExceptionHandler::HttpVersionNotSupported::~HttpVersionNotSupported() throw() {}