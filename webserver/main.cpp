#include "Webserv.hpp"

int main(int ac, char** av)
{
    if (ac != 2)
    {
        std::cout << "usage: \n\t./webserv conf.txt" << std::endl;
        return (1);
    }
    try
    {
        Webserv webserv = Webserv(std::string(av[1]));
        webserv.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return (0);
}