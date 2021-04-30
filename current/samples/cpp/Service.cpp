#include "Service.h"
#include <string>
#pragma warning( disable: 4819 )

using namespace std;

void Service::initRestOpHandlers() {
    _listener.support(methods::GET, std::bind(&Service::handleGet, this, std::placeholders::_1));
}

void Service::handleGet(http_request raquest) {

    std::vector<utility::string_t> path = requestPath(raquest);
    std::string pathEndpoint = utility::conversions::to_utf8string(path[0]);

    raquest.extract_json().then([=](pplx::task<json::value> task)
        {
            try
            {
                json::value val = task.get();
                if (pathEndpoint == "startReadingPositions") 
                {
                    sensor.set_work(true);
                    auto http_get_vars = uri::split_query(raquest.request_uri().query());
                    auto found_shape = http_get_vars.find(U("shape"));
                    if (found_shape != end(http_get_vars)) 
                    {
                        auto shape = found_shape->second;
                        sensor.start_reading_data_from_sensor(utility::conversions::to_utf8string(shape));
                    }
                    else
                    {
                        sensor.start_reading_data_from_sensor("square");
                    }
                    
                    raquest.reply(status_codes::OK);
                }
                if (pathEndpoint == "stopReadingPositions")
                {
                    sensor.stop_reading_data_from_sensor();
                    raquest.reply(status_codes::OK);
                }
            }
            catch (std::exception& e) {
                raquest.reply(status_codes::BadRequest);
            }
        });
    
}
