#ifndef SERVICE_H_INCLUDED
#define SERVICE_H_INCLUDED
#include "Controller.h"
#include "URG_touch_screen.h"

class Service : public Controller {
public:
    Service(const utility::string_t& address, const utility::string_t& port) : Controller(address, port) {}
    ~Service() {}
    void handleGet(http_request message);
    void initRestOpHandlers() override;
private:
    URG_touch_screen sensor;
};
#endif // SERVICE_H_INCLUDED
