#pragma once
#include "Urg_driver.h"

class URG_touch_screen {
	
	class Point {
		public:
			float x;
			float y;
			Point(){
				x = 0;
				y = 0;
			}	
	};
	
	bool work;
	bool calibratedLT, calibratedRB;
	Point p_LT, p_RT, p_LB, p_RB;
	int min_angle_degree, max_angle_degree;
	qrk::Urg_driver urg;

public:
	bool calibration_end;
	URG_touch_screen();
	void start_reading_data_from_sensor(int width, int height, double pixel_size, int screen_width, int screen_height, std::string* screens_urls, bool tracking_point_mode);
	void stop_reading_data_from_sensor();
	void send_request_to_xinuk(int x, int y, int width, int height, int screen_width, int screen_height, std::string* screens_urls);
	void set_work(bool work_value);
	bool calibrate(int width, int height, double pixel_size, bool is_point_00);
	Point find_min(Point* points, bool is_x);
	float area_triangle(float x1, float y1, float x2, float y2, float x3, float y3);
	bool is_piont_in_rectangle(Point P1, Point P2, Point P3, Point P4, float x, float y);
	float distance_point_from_streight(Point P1, Point P2, float x, float y);
};
