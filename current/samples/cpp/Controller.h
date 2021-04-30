#pragma once
#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED
#include <cpprest/http_listener.h>
#include <pplx/pplxtasks.h>
#include <string>
#include <cpprest/http_msg.h>
using namespace web;
using namespace http;
using namespace http::experimental::listener;

class Controller {
public:
    Controller(const utility::string_t& address, const utility::string_t& port);
    ~Controller();
    void setEndpoint(const utility::string_t& mount_point);
    std::string endpoint() const;
    pplx::task<void> accept();
    pplx::task<void> shutdown();

    virtual void initRestOpHandlers() = 0;

    std::vector<utility::string_t> requestPath(const http_request& message);
protected:
    http_listener _listener;
private:
    uri_builder endpointBuilder;
};

#endif // CONTROLLER_H_INCLUDED