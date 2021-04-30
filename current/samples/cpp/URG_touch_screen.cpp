#include "Connection_information.h"
#include "math_utilities.h"
#include "URG_touch_screen.h"
#include <iostream>
#include <cpr/cpr.h>

using namespace qrk;
using namespace std;

URG_touch_screen::URG_touch_screen() {

	// Set work
	work = true;

	// Create shapes map
	shapes["square"] = 0;
	shapes["vertical"] = 1;
	shapes["horizontal"] = 2;
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

void  URG_touch_screen::start_reading_data_from_sensor(string shape)
{

	// Gets measurement data
	// Case where the measurement range (start/end steps) is defined
	urg.set_scanning_parameter(urg.deg2step(-90), urg.deg2step(0), 0);
	
	
	long screen_max_x = 640;
	long screen_max_y = 640;

	switch (shapes[shape])
	{
	case 0: //square
		screen_max_x = 640;
		screen_max_y = 640;
		break;
	case 1: //vertical
		screen_max_x = 1280;
		screen_max_y = 320;
		break;
	case 2: //horizontal
		screen_max_x = 320;
		screen_max_y = 1280;
		break;
	default:
		screen_max_x = 640;
		screen_max_y = 640;
		break;
	}

	long min_distance = urg.min_distance();
	long max_distance = sqrt(pow(screen_max_x, 2) + pow(screen_max_y, 2));

	long accuracy = 10;
	long before_x = 0;
	long before_y = 0;

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

			x = round(x / accuracy);
			y = round(y / accuracy);

			if ((x == before_x && y == before_y) || x < 0 || y < 0) {
				continue;
			}

			before_x = x;
			before_y = y;
			cout << "(" << x << ", " << y << ")" << endl;
			send_request_to_xinuk(x, y, shape);
		}
	}
}

void URG_touch_screen::send_request_to_xinuk(long x, long y, string shape) {


		string port = ":8000";

		string url_1 = "http://192.168.1.87";
		string url_2 = "http://192.168.1.172";
		string url_3 = "http://192.168.1.205";
		string url_4 = "http://192.168.1.196";

		url_1.append(port);
		url_2.append(port);
		url_3.append(port);
		url_4.append(port);

		long one_screen_x = 32;
		long one_screen_y = 32;

		char query[50];
		int n;

		cpr::Response r;
	
		switch (shapes[shape])
		{
		case 0: //squere
			if (y < 32)
			{
				if (x < 32)
				{
					//send to 1
					n = sprintf(query, "/%d/%d", x, y);
					r = cpr::Get(cpr::Url{url_1.append(query)});
					cout<<r.status_code<<endl;
				}
				else
				{
					//send to 2
					n = sprintf(query, "/%d/%d", x - one_screen_x, y);
					r = cpr::Get(cpr::Url{url_2.append(query)});
					cout<<r.status_code<<endl;
				}
			}
			else
			{
				if (x < 32)
				{
					//send to 3
					n = sprintf(query, "/%d/%d", x, y - one_screen_y);
					r = cpr::Get(cpr::Url{url_3.append(query)});
					cout<<r.status_code<<endl;
				}
				else
				{
					//send to 4
					n = sprintf(query, "/%d/%d", x - one_screen_x, y - one_screen_y);
					r = cpr::Get(cpr::Url{url_4.append(query)});
					cout<<r.status_code<<endl;
				}
			}
			break;
		case 1: //vertical
			if (y < 32)
			{
				//send to 1
					n = sprintf(query, "/%d/%d", x, y);
					r = cpr::Get(cpr::Url{url_1.append(query)});
					cout<<r.status_code<<endl;
				break;
			}
			if (y < 64)
			{
				//send to 2
					n = sprintf(query, "/%d/%d", x, y - one_screen_y);
					r = cpr::Get(cpr::Url{url_2.append(query)});
					cout<<r.status_code<<endl;
				break;
			}
			if (y < 96)
			{
				//send to 3
					n = sprintf(query, "/%d/%d", x, y - 2 * one_screen_y);
					r = cpr::Get(cpr::Url{url_3.append(query)});
					cout<<r.status_code<<endl;
				break;
			}
			// send to 4
					n = sprintf(query, "/%d/%d", x, y - 3 * one_screen_y);
					r = cpr::Get(cpr::Url{url_4.append(query)});
					cout<<r.status_code<<endl;
			break;
		case 2: //horizontal
			if (x < 32)
			{
				//send to 1
					n = sprintf(query, "/%d/%d", x, y);
					r = cpr::Get(cpr::Url{url_1.append(query)});
					cout<<r.status_code<<endl;
				break;
			}
			if (x < 64)
			{
				//send to 2
					n = sprintf(query, "/%d/%d", x - one_screen_x, y );
					r = cpr::Get(cpr::Url{url_2.append(query)});
					cout<<r.status_code<<endl;
				break;
			}
			if (x < 96)
			{
				//send to 3
					n = sprintf(query, "/%d/%d", x - 2 * one_screen_x, y );
					r = cpr::Get(cpr::Url{url_3.append(query)});
					cout<<r.status_code<<endl;
				break;
			}
			// send to 4
					n = sprintf(query, "/%d/%d", x - 3 * one_screen_x, y );
					r = cpr::Get(cpr::Url{url_4.append(query)});
					cout<<r.status_code<<endl;
			break;
		default:
			break;
		}

	
}

void  URG_touch_screen::stop_reading_data_from_sensor() {
	work = false;
}

void  URG_touch_screen::set_work(bool work_value) {
	work = work_value;
}
