#include "RequestHandler.hpp"

RequestHandler::RequestHandler() {}

RequestHandler::RequestHandler(const RequestHandler& other)
{
    (void)other;
}

RequestHandler::~RequestHandler() {}

RequestHandler& RequestHandler::operator=(const RequestHandler& other)
{
    if (this != &other)
    {
        // copy members here
        (void)other;
    }
    return *this;
}
