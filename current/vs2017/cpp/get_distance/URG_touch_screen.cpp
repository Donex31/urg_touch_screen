#include "Connection_information.h"
#include "math_utilities.h"
#include "URG_touch_screen.h"
#include <iostream>
#include <curl/curl.h>

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
	urg.set_scanning_parameter(urg.deg2step(0), urg.deg2step(+90), 0);
	
	
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

	CURL* curl;
	CURLcode res;
	curl = curl_easy_init();

	if (curl) 
	{
		string port = ":8000";

		string url_1 = "http://192.168.100.180";
		string url_2 = "http://192.168.100.185";
		string url_3 = "http://192.168.100.191";
		string url_4 = "http://192.168.100.192";

		url_1.append(port);
		url_2.append(port);
		url_3.append(port);
		url_4.append(port);

		char query[50];
		int n = sprintf(query, "/%d/%d", x, y);

		switch (shapes[shape])
		{
		case 0: //squere
			if (y < 32)
			{
				if (x < 32)
				{
					//send to 1
					curl_easy_setopt(curl, CURLOPT_URL, url_1.append(query));
					res = curl_easy_perform(curl);
					curl_easy_cleanup(curl);
				}
				else
				{
					//send to 2
					curl_easy_setopt(curl, CURLOPT_URL, url_2.append(query));
					res = curl_easy_perform(curl);
					curl_easy_cleanup(curl);
				}
			}
			else
			{
				if (x < 32)
				{
					//send to 3
					curl_easy_setopt(curl, CURLOPT_URL, url_3.append(query));
					res = curl_easy_perform(curl);
					curl_easy_cleanup(curl);
				}
				else
				{
					//send to 4
					curl_easy_setopt(curl, CURLOPT_URL, url_4.append(query));
					res = curl_easy_perform(curl);
					curl_easy_cleanup(curl);
				}
			}
			break;
		case 1: //vertical
			if (y < 32)
			{
				//send to 1
				curl_easy_setopt(curl, CURLOPT_URL, url_1.append(query));
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				break;
			}
			if (y < 64)
			{
				//send to 2
				curl_easy_setopt(curl, CURLOPT_URL, url_2.append(query));
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				break;
			}
			if (y < 96)
			{
				//send to 3
				curl_easy_setopt(curl, CURLOPT_URL, url_3.append(query));
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				break;
			}
			// send to 4
			curl_easy_setopt(curl, CURLOPT_URL, url_4.append(query));
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			break;
		case 2: //horizontal
			if (x < 32)
			{
				//send to 1
				curl_easy_setopt(curl, CURLOPT_URL, url_1.append(query));
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				break;
			}
			if (x < 64)
			{
				//send to 2
				curl_easy_setopt(curl, CURLOPT_URL, url_2.append(query));
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				break;
			}
			if (x < 96)
			{
				//send to 3
				curl_easy_setopt(curl, CURLOPT_URL, url_3.append(query));
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				break;
			}
			// send to 4
			curl_easy_setopt(curl, CURLOPT_URL, url_4.append(query));
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			break;
		default:
			break;
		}
	}

	
}

void  URG_touch_screen::stop_reading_data_from_sensor() {
	work = false;
}

void  URG_touch_screen::set_work(bool work_value) {
	work = work_value;
}