#pragma once
#include "Urg_driver.h"

class URG_touch_screen {
	bool work;
	qrk::Urg_driver urg;

public:
	URG_touch_screen();
	void start_reading_data_from_sensor(int width, int height, double pixel_size, int screen_width, int screen_height);
	void stop_reading_data_from_sensor();
	void send_request_to_xinuk(long x, long y, int width, int height, int screen_width, int screen_height);
	void set_work(bool work_value);
};
