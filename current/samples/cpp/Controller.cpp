#include "Controller.h"

Controller::Controller(const utility::string_t& naddress, const utility::string_t& nport) {
    this->endpointBuilder.set_host(naddress);
    this->endpointBuilder.set_port(nport);
    this->endpointBuilder.set_scheme(utility::conversions::to_string_t("http"));
}

Controller::~Controller() {
}
void Controller::setEndpoint(const utility::string_t& mount_point)
{
    endpointBuilder.set_path(mount_point);
    _listener = http_listener(endpointBuilder.to_uri());
}

pplx::task<void> Controller::accept() {
    initRestOpHandlers();
    return _listener.open();
}
pplx::task<void> Controller::shutdown() {
    return _listener.close();
}

std::vector<utility::string_t> Controller::requestPath(const http_request& message)
{
    auto relativePath = uri::decode(message.relative_uri().path());
    return uri::split_path(relativePath);
}