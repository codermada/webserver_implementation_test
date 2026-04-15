#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

class RequestHandler
{
public:
    RequestHandler();
    RequestHandler(const RequestHandler& other);
    ~RequestHandler();

    RequestHandler& operator=(const RequestHandler& other);
};

#endif
