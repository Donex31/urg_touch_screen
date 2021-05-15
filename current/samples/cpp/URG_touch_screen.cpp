#include "Connection_information.h"
#include "math_utilities.h"
#include "URG_touch_screen.h"
#include <iostream>
#include <cpr/cpr.h>
#include <tuple>
#include <vector>
#include <cmath>

using namespace qrk;
using namespace std;

URG_touch_screen::URG_touch_screen() {

	// Set work
	work = true;

	// Get connection information
	Connection_information information(true);

	// Connect to ugr device

	if (!urg.open(information.device_or_ip_name(),
		information.baudrate_or_port_number(),
		information.connection_type())) {
		cout << "Urg_driver::open(): "
			<< information.device_or_ip_name() << ": " << urg.what() << endl;
		return;
	}
}

void  URG_touch_screen::start_reading_data_from_sensor(int width, int height, double pixel_size, int screen_width, int screen_height)
{

	// Gets measurement data
	// Case where the measurement range (start/end steps) is defined
	urg.set_scanning_parameter(urg.deg2step(-90), urg.deg2step(0), 0);
	
	int to_mm_factor = 10;
	
	long screen_max_x = width * pixel_size * to_mm_factor;
	long screen_max_y = height * pixel_size * to_mm_factor;

	long min_distance = urg.min_distance();
	long max_distance = sqrt(pow(screen_max_x, 2) + pow(screen_max_y, 2));
	
	int screen_matrix[height][width] = {0};

	urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);

	while (work) {
		vector<long> data;
		long time_stamp = 0;
		if (!urg.get_distance(data, &time_stamp)) {
			cout << "Urg_driver::get_distance(): " << urg.what() << endl;
		}

		// Prints the X-Y coordinates for all the measurement points

		size_t data_n = data.size();

		for (size_t i = 0; i < data_n; ++i) {

			long l = data[i];
			if ((l <= min_distance) || (l >= max_distance)) {
				continue;
			}

			double radian = urg.index2rad(i);
			long x = static_cast<long>(l * cos(radian));
			long y = static_cast<long>(l * sin(radian));

			if ((x >= screen_max_x) || (y >= screen_max_y)) {
				continue;
			}

			x = round(x / pixel_size);
			y = round(y / pixel_size);

			if (x < 0 || y < 0 || screen_matrix[y][x]) {
				continue;
			}

			screen_matrix[y][x] = 1;
			cout << "(" << x << ", " << y << ")" << endl;
			send_request_to_xinuk(x, y, width, height, screen_width, screen_height);
		}
	}
}

void URG_touch_screen::send_request_to_xinuk(long x, long y, int width, int height, int screen_width, int screen_height) {


		string screen_urls[] = 
		{
			"http://192.168.1.87:8000",
			"http://192.168.1.172:8000",
			"http://192.168.1.205:8000",
			"http://192.168.1.196:8000"
		}

		int number_of_columns = celi(width/screen_width);
		int number_of_rows = celi(height/screen_height);
		
		int column_number = celi(x/(screen_width-1));
		int row_number = celi(y/(screen_height-1));
		
		int screen_index = (row_number - 1) * number_of_columns + column_number;
		char query[50];
		int n;

		cpr::Response r;
		
		int screen_x = x - (column_number - 1) * screen_width;
		int screen_y = y - (row_number - 1) * screen_height;
	
		n = sprintf(query, "/%d/%d", screen_x, screen_y);
		r = cpr::Get(cpr::Url{screens_urls[screen_index-1].append(query)});
		cout<<r.status_code<<endl;
}

void  URG_touch_screen::stop_reading_data_from_sensor() {
	work = false;
}

void  URG_touch_screen::set_work(bool work_value) {
	work = work_value;
}
