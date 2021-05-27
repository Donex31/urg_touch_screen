#pragma once
#include "Urg_driver.h"

class URG_touch_screen {
	bool work;
	int correct_x;
	int correct_y;
	qrk::Urg_driver urg;

public:
	URG_touch_screen();
	void start_reading_data_from_sensor(int width, int height, double pixel_size, int screen_width, int screen_height, std::string* screens_urls);
	void stop_reading_data_from_sensor();
	void send_request_to_xinuk(int x, int y, int width, int height, int screen_width, int screen_height, std::string* screens_urls);
	void set_work(bool work_value);
	bool calibrate();
};
