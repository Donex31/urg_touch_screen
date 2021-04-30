#pragma once
#include "Urg_driver.h"
#include <map>

class URG_touch_screen {
	bool work;
	qrk::Urg_driver urg;
	std::map<std::string, int> shapes;

public:
	URG_touch_screen();
	void start_reading_data_from_sensor(std::string shape);
	void stop_reading_data_from_sensor();
	void send_request_to_xinuk(long x, long y, std::string shape);
	void set_work(bool work_value);
};