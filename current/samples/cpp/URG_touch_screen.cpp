#include "Connection_information.h"
#include "math_utilities.h"
#include "URG_touch_screen.h"
#include <iostream>
#include <cpr/cpr.h>
#include <tuple>
#include <vector>
#include <cmath>
#include <cstring>

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

void  URG_touch_screen::start_reading_data_from_sensor(int width, int height, double pixel_size, int screen_width, int screen_height, string* screens_urls)
{
	// Gets measurement data
	// Case where the measurement range (start/end steps) is defined
	urg.set_scanning_parameter(urg.deg2step(-90), urg.deg2step(0), 0);
	
	int pixel_size_mm = pixel_size * 10;
	
	long screen_max_x = width * pixel_size_mm;
	long screen_max_y = height * pixel_size_mm;

	long min_distance = urg.min_distance();
	long max_distance = sqrt(pow(screen_max_x, 2) + pow(screen_max_y, 2));
	
	int screen_matrix[height][width] = {0};

	urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);

	while (work) {
		vector<long> data;
		long time_stamp = 0;
		if (!urg.get_distance(data, &time_stamp)) {
			cout << "Urg_driver::get_distance(): " << urg.what() << endl;
			urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);
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

			x = round(x / pixel_size_mm);
			y = round(y / pixel_size_mm);

			if (x < 0 || y < 0 || screen_matrix[y][x]) {
				continue;
			}

			screen_matrix[y][x] = 1;
			cout << "(" << x << ", " << y << ")" << endl;
			send_request_to_xinuk(x, y, width, height, screen_width, screen_height, screens_urls);
		}
	}
}

void URG_touch_screen::send_request_to_xinuk(long x, long y, int width, int height, int screen_width, int screen_height, string* screens_urls) {

		int number_of_columns = ceil(width/screen_width);
		int number_of_rows = ceil(height/screen_height);
		
		
		int column_number = x==0 ? 1 : ceil((double)x/(double)(screen_width-1));
		int row_number = y==0 ? 1 :  ceil((double)y/(double)(screen_height-1));
		
		int screen_index = (row_number - 1) * number_of_columns + column_number;
		
		cout << "Number Of Columns: " << number_of_columns <<endl; 
		cout << "Number Of Rows: " << number_of_rows <<endl; 
		cout << "Column Number: " << column_number <<endl; 
		cout << "Row Number: " << row_number <<endl; 
		cout << "Screen Index: " << screen_index <<endl; 

		
		string query;

		cpr::Response r;
		
		int screen_x = x - (column_number - 1) * screen_width;
		int screen_y = y - (row_number - 1) * screen_height;

		query = "";
		query = screens_urls[screen_index-1] + "/" + to_string(screen_x) + "/" + to_string(screen_y);
		cout<<"Query: "<< query<<endl;
		r = cpr::Get(cpr::Url{query});
		cout<<"Status Code: "<<r.status_code<<endl;
}

void  URG_touch_screen::stop_reading_data_from_sensor() {
	work = false;
}

void  URG_touch_screen::set_work(bool work_value) {
	work = work_value;
}
