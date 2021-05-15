#include "Service.h"
#include <string>
#pragma warning( disable: 4819 )

using namespace std;

void Service::initRestOpHandlers() {
    _listener.support(methods::GET, std::bind(&Service::handleGet, this, std::placeholders::_1));
    _listener.support(methods::POST, std::bind(&Service::handlePost, this, std::placeholders::_1));
}

void Service::handleGet(http_request raquest) {
    
    cout<<"Handle get"<<endl;

    vector<utility::string_t> path = requestPath(raquest);
    string pathEndpoint = utility::conversions::to_utf8string(path[0]);
    
    raquest.extract_json().then([=](pplx::task<json::value> task)
        {
            try
            {
                json::value val = task.get();
                if (pathEndpoint == "stopReadingPositions")
                {
                    cout << "get /stopReadingPositions"<<endl;
                    sensor.stop_reading_data_from_sensor();
                    raquest.reply(status_codes::OK);
                }
                else 
                {
                    raquest.reply(status_codes::BadRequest);
                }
            }
            catch (exception& e) {
                raquest.reply(status_codes::BadRequest);
            }
        });
    
}

void Service::handlePost(http_request message) {

    cout<<"Handle post"<<endl;

    vector<utility::string_t> path = requestPath(message);
    string pathEndpoint = utility::conversions::to_utf8string(path[0]);
    
    message.extract_json().then([=](pplx::task<json::value> task)
        {
            try
            {
                json::value val = task.get();
                if (pathEndpoint == "startReadingPositions") {
                    cout << "POST /startReadingPositions"<<endl;
                    int width = val[U("width")].as_number().to_int32();
                    int height = val[U("height")].as_number().to_int32();
                    double pixel_size = val[U("pixelSize")].as_number().to_double();
                    int screen_width = val[U("screenWidth")].as_number().to_int32();
                    int screen_height = val[U("sreenHeight")].as_number().to_int32();

                    cout << "width = " << width << " height = " << height << " pixelSize = " << pixel_size << " screenWidth = " << screen_width << " screenHeight = " << screen_height << endl;

                    sensor.start_reading_data_from_sensor(width, height, pixel_size, screen_width, screen_height);
                    message.reply(status_codes::OK);

                }
                else {
                    message.reply(status_codes::BadRequest);
                }

            }
            catch (exception& e) {
                cerr << e.what();
                message.reply(status_codes::BadRequest);
            }
        });
    
}
