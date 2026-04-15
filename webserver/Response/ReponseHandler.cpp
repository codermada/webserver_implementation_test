#include "ReponseHandler.hpp"

ReponseHandler::ReponseHandler() {}

ReponseHandler::ReponseHandler(const ReponseHandler& other)
{
    (void)other;
}

ReponseHandler::~ReponseHandler() {}

ReponseHandler& ReponseHandler::operator=(const ReponseHandler& other)
{
    if (this != &other)
    {
        // copy members here
        (void)other;
    }
    return *this;
}
