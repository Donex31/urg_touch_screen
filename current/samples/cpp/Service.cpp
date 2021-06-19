#include "Service.h"
#include <string>
#include <future>
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
                cerr << e.what();
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
                    if(!sensor.calibration_end){
                        sensor.stop_reading_data_from_sensor();
                        message.reply(status_codes::NoContent , "Please make calibration first");
                    }
                    int width = val[U("width")].as_number().to_int32();
                    // cout << "width = " << width << endl;
                    int height = val[U("height")].as_number().to_int32();
                    // cout << "height = " << height << endl;
                    double pixel_size = val[U("pixelSize")].as_number().to_double();
                    // cout << "pixelSize = " << pixel_size << endl;
                    int screen_width = val[U("screenWidth")].as_number().to_int32();
                    // cout << "screenWidth = " << screen_width << endl;
                    int screen_height = val[U("screenHeight")].as_number().to_int32();
                    // cout << "sreenHeight = " << screen_height << endl;
                    auto urls_array = val[U("hosts")].as_array();
                    string* screens_urls = new string[urls_array.size()];
                    
                    for(size_t i = 0; i < urls_array.size(); ++i)
                    {
                        screens_urls[i] = utility::conversions::to_utf8string(urls_array[i].as_string());
                        cout << "Host no "<< i <<": "<< screens_urls[i] << endl;
                    }

                    cout << "width = " << width << " height = " << height << " pixelSize = " << pixel_size << " screenWidth = " << screen_width << " screenHeight = " << screen_height << endl;
                    
                    bool is_tracking_mode = val[U("isTrackingMode")].as_bool();
                    // cout << "sreenHeight = " << screen_height << endl;
                    
                    auto f = async(launch::async, &URG_touch_screen::start_reading_data_from_sensor, std::ref(sensor), width, height, pixel_size, screen_width, screen_height, screens_urls, is_tracking_mode);
                    message.reply(status_codes::OK);

                }
                else if (pathEndpoint == "calibration") {
                    cout << "POST /calibration" << endl;
                    int width = val[U("width")].as_number().to_int32();
                    // cout << "width = " << width << endl;
                    int height = val[U("height")].as_number().to_int32();
                    // cout << "height = " << height << endl;
                    double pixel_size = val[U("pixelSize")].as_number().to_double();
                    // cout << "pixelSize = " << pixel_size << endl;
                    bool is_point_00 = val[U("isZeroZero")].as_bool();
                    if (sensor.calibrate(width, height, pixel_size, is_point_00)) 
                    {
                        message.reply(status_codes::OK);
                    }
                    else {
                        message.reply(status_codes::NoContent , "No marker in area!");
                    }

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
