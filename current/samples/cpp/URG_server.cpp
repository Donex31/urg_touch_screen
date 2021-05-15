#include <iostream>
#include "Service.h"

int main()
{
    std::string localhost = "192.168.43.4";
    std::string port = "8888";
    std::string endpoint = "/";

    Service serv(utility::conversions::to_string_t(localhost), utility::conversions::to_string_t(port));
    serv.setEndpoint(utility::conversions::to_string_t(endpoint));
    serv.accept().wait();

    std::cout << "Server start at http//" + localhost + ":" + port + endpoint << std::endl;

    std::getchar();
    return 0;
}
