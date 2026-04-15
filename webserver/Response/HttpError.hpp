#ifndef HTTPERROR_HPP
#define HTTPERROR_HPP


#include <string>
#include <sstream>

#include "File.hpp"
#include "Validator.hpp"

class HttpError
{
  private:
    HttpError();
    HttpError(const HttpError& other);
    HttpError& operator=(const HttpError& other);

  public:
    ~HttpError();
    static std::string getStatusMessage(int code);
    static std::string error(int code, const std::string& errorPath);
};

#endif
