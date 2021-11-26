#include "Connection_information.h"
#include "math_utilities.h"
#include "URG_touch_screen.h"
#include <iostream>
#include <cpr/cpr.h>
#include <tuple>
#include <vector>
#include <cmath>
#include <cstring>
#include <fstream>
#include "kf.h"
#include "ekf.h"
//#include "ukf.h"

using namespace qrk;
using namespace std;

//class MyUKF : public UKF
//{
//public:
//	virtual colvec f(const colvec& x, const colvec& u) {
//		colvec xk(nStates_);
//		mat A(nStates_, nStates_);
//
//		A = A.eye();
//
//		xk = A * x;;
//		return xk;
//	}
//
//	virtual colvec h(const colvec& x) {
//		colvec zk(nOutputs_);
//		zk(0) = x(0);
//		zk(1) = x(1);
//		return zk;
//	}
//
//};

class MyEKF : public EKF
{
public:
	virtual colvec f(const colvec& x, const colvec& u) {
		colvec xk(nStates_);
		mat A(nStates_, nStates_);

		A = A.eye();

		xk = A * x;;
		return xk;
	}

	virtual colvec h(const colvec& x) {
		colvec zk(nOutputs_);
		zk(0) = x(0);
		zk(1) = x(1);
		return zk;
	}

	static const double dt;

};

URG_touch_screen::URG_touch_screen() {

	// Set work
	work = true;
	
	// Set calibration
	calibratedLT = false;
	calibratedRB = false;
	calibration_end = false;

	// Set deflaut corectors
	// L - Left
	// R - Right
	// T - Top 
	// B - Bottom
	p_LT = Point();
	p_LB = Point();
	p_RT = Point();
	p_RB = Point();
	
	// Set deflout angles 
	min_angle_degree = 0;
	max_angle_degree = 90;

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

void  URG_touch_screen::start_reading_data_from_sensor(int width, int height, double pixel_size, int screen_width, int screen_height, string* screens_urls, bool tracking_point_mode)
{

	ofstream myfile;
	myfile.open("output.txt");
	// Gets measurement data
	// Case where the measurement range (start/end steps) is defined
	work = true;
	urg.set_scanning_parameter(urg.deg2step(90), urg.deg2step(0), 0);
	
	int pixel_size_mm = (int)round(pixel_size * 10);
	
	long screen_max_x = (width * pixel_size_mm);
	long screen_max_y = (height * pixel_size_mm);

	long min_distance = (long)round(sqrt(pow(p_LT.x, 2) + pow(p_LT.y, 2)));
	long max_distance = (long)round(sqrt(pow(p_RB.x, 2) + pow(p_RB.y, 2)));

	int scan_times = Urg_driver::Infinity_times;
	//int scan_times = 50;
	int skip_scan = 0;
	
	cout << "min_distance: " << min_distance <<endl; 
	cout << "max_distance: " << max_distance <<endl; 
	cout << "min_angle_degree: " << min_angle_degree <<endl; 
	cout << "max_angle_degree: " << max_angle_degree <<endl; 

	vector<vector<int>> screen_matrix(height);
	for (int i = 0; i < height; i++)
		screen_matrix[i].resize(width);

	//Kalman Filter
	//int n_states = 2;
	//int n_outputs = 2;
	//mat A(n_states, n_outputs), B(n_states, n_outputs), H(n_states, n_outputs), Q(n_states, n_outputs), R(n_states, n_outputs);

	//A << 1 << 0 << endr
	//	<< 0 << 1 << endr;

	//H << 1 << 0 << endr
	//	<< 0 << 1 << endr;

	//std::cout << "A: \n" << A << std::endl;
	//std::cout << "H: \n" << H << std::endl;

	//B = B.zeros();

	//double alpha = 1.5;
	//double sigma = 3;

	//Q = alpha * Q.eye();

	//R = sigma * R.eye();

	//std::cout << "B: \n" << B << std::endl;
	//std::cout << "Q: \n" << Q << std::endl;
	//std::cout << "R: \n" << R << std::endl;

	//colvec x0(2);
	//x0 << 0 << 8;

	//mat P0(2, 2);
	//P0 = 3 * P0.eye();

	//KF kalman;
	//MyEKF myekf;
	//MyUKF myukf;

	//kalman.InitSystem(A, B, H, Q, R);
	//kalman.InitSystemState(x0);
	//kalman.InitStateCovariance(P0);

	//myekf.InitSystem(n_states, n_outputs, Q, R);
	//myekf.InitSystemState(x0);
	//myekf.InitSystemStateCovariance(P0);

	//myukf.InitSystem(n_states, n_outputs, Q, R);
	//myukf.InitSystemState(x0);
	//myukf.InitSystemStateCovariance(P0);

	//colvec z(2);
	//colvec u(2);

	// No inputs, system subjects only to random perturbation
	//u = u.zeros();
	//

	urg.start_measurement(Urg_driver::Distance, scan_times, skip_scan);
	int t = 0;
	while (work) {
		t++;
		if (!t % 11) {
			for (int i = 0; i < height; i++)
				for (size_t j = 0; j < width; j++)
				{
					if (screen_matrix[i][j] < 8)
						screen_matrix[i][j] = 0;
				}
		}

		vector<long> data;
		long time_stamp = 0;
		if (!urg.get_distance(data, &time_stamp)) {
			cout << "Urg_driver::get_distance(): " << urg.what() << endl;
			urg.start_measurement(Urg_driver::Distance, scan_times, skip_scan);
		}

		// Prints the X-Y coordinates for all the measurement points

		size_t data_n = data.size();

		for (size_t i = 0; i < data_n; ++i) {			
			long l = data[i];
			if ((l <= min_distance) || (l >= max_distance)) {
				continue;
			}

			double radian = urg.index2rad(i);


			long y = static_cast<long>(l * cos(radian));
			long x = static_cast<long>(l * sin(radian));

			if (x < 0 || y < 0) {
				continue;
			};

			/*if(!is_piont_in_rectangle(p_LT ,p_RT, p_LB, p_RB, x, y)){
				continue;
			}*/
			
			float distance_x = distance_point_from_streight(p_LT, p_LB, x, y);
			float distance_y = distance_point_from_streight(p_LT, p_RT, x, y);

			int x_index = (int)round(distance_x / pixel_size_mm);
			int y_index = (int)round(distance_y / pixel_size_mm);

			if (x_index >= width || y_index >= height) {
				continue;
			}

			screen_matrix[y_index][x_index] = screen_matrix[y_index][x_index] + 1;

			if (screen_matrix[y_index][x_index] != 10) {
				continue;
			}

			//z(0, 0) = x_index;
			//z(1, 0) = y_index;

			//kalman.Kalmanf(z, u);
			//colvec* z = kalman.GetCurrentEstimatedOutput();

			//myekf.EKalmanf(z, u);
			//colvec* z = myekf.GetCurrentEstimatedOutput();

			//myukf.UKalmanf(z, u);
			//colvec* z = myukf.GetCurrentEstimatedOutput();

			//x_index = z->at(0, 0);
			//y_index = z->at(1, 0);

			cout << "(" << x_index << ", " << y_index << ")" << endl;
			myfile << "(" << x_index << ", " << y_index << "),";
			auto f = async(launch::async, &URG_touch_screen::send_request_to_xinuk, this, x_index, y_index, width, height, screen_width, screen_height, screens_urls);
		}
	}
	myfile.close();
}

void URG_touch_screen::send_request_to_xinuk(int x, int y, int width, int height, int screen_width, int screen_height, string* screens_urls) {

		int number_of_columns = (int)ceil(width/screen_width);
		int number_of_rows = (int)ceil(height/screen_height);
		
		int column_number = x==0 ? 1 : (int)ceil((double)x/((double)screen_width - 1));
		int row_number = y==0 ? 1 :  (int)ceil((double)y/((double)screen_height - 1));
		
		int screen_index = (row_number - 1) * number_of_columns + column_number;
		
		//cout << "Number Of Columns: " << number_of_columns <<endl; 
		//cout << "Number Of Rows: " << number_of_rows <<endl; 
		//cout << "Column Number: " << column_number <<endl; 
		//cout << "Row Number: " << row_number <<endl; 
		//cout << "Screen Index: " << screen_index <<endl; 

		string query;

		cpr::Response r;
		
		int screen_x = x - (column_number - 1) * screen_width;
		int screen_y = y - (row_number - 1) * screen_height;

		query = "";
		query = screens_urls[screen_index-1] + "/" + to_string(screen_x) + "/" + to_string(screen_y);
		// cout<<"Query: "<< query<<endl;
		r = cpr::Get(cpr::Url{query});
		// cout<<"Status Code: "<<r.status_code<<endl;
}

bool URG_touch_screen::calibrate(int width, int height, double pixel_size, bool is_point_00) {
	
	min_angle_degree = 0;
	max_angle_degree = 90;
	
	urg.set_scanning_parameter(urg.deg2step(max_angle_degree), urg.deg2step(min_angle_degree), 0);

	urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);
	
	long buffor_size = 200;
	
	cout << "is_point_00: " << is_point_00 << endl;
	cout << "width: " << width << endl;
	cout << "height: " << height << endl;
	cout << "pixel_size: " << pixel_size << endl;
	
	long min_distance = urg.min_distance();
	
	if(!is_point_00){
		int pixel_size_mm = (int)round(pixel_size * 10);

		long screen_max_x = (width * pixel_size_mm);
		long screen_max_y = (height * pixel_size_mm);

		min_distance = (long) round(sqrt(pow(screen_max_x, 2) + pow(screen_max_y, 2)));
		buffor_size = (long) (buffor_size + round(sqrt(pow(screen_max_x, 2) + pow(screen_max_y, 2))));
	}
	
	long max_distance = buffor_size;

	cout << "min_distance: " << min_distance << endl;
	cout << "max_distance: " << max_distance << endl;


	int i = 0;
	int it = 0;
	
	p_LT.x = 0;
	p_LT.y = 0;
	p_RB.x = 0;
	p_RB.y = 0;
	while (i < 50)
	{
		i++;
		vector<long> data;
		long time_stamp = 0;
		if (!urg.get_distance(data, &time_stamp)) {
			cout << "Urg_driver::get_distance(): " << urg.what() << endl;
			urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);
		}

		size_t data_n = data.size();

		for (size_t i = 0; i < data_n; ++i) {

			long l = data[i];
			if ((l <= min_distance) || (l >= max_distance)) {
				continue;
			}

			double radian = urg.index2rad(i);
			long y = static_cast<long>(l * cos(radian));
			long x = static_cast<long>(l * sin(radian));

			if ((x >= buffor_size) || (y >= buffor_size) || x < 0 || y < 0) {
				continue;
			}

			if(is_point_00){
				p_LT.x = p_LT.x + (float)x;
				p_LT.y = p_LT.y + (float)y;
			}
			else
			{
				p_RB.x = p_RB.x + (float)x;
				p_RB.y = p_RB.y + (float)y;
			}
			
			if(it != 0)
			{
				if(is_point_00)
				{
					p_LT.x = p_LT.x / it;
					p_LT.y = p_LT.y / it;
					cout << "x_LT: " << p_LT.x << " y_LT: " << p_LT.y << endl;
					calibratedLT  = true;
				}
				else
				{
					p_RB.x = p_RB.x / it;
					p_RB.y = p_RB.y / it;
					cout << "x_RB: " << p_RB.x << " y_RB: " << p_RB.y << endl;
					calibratedRB  = true;
				}
			}
			
		}
		
		if(is_point_00 && calibratedLT)
			break;
		if(!is_point_00 && calibratedRB)
			break;
	}

	//calibratedLT = true; 
	//calibratedRB = true;
	//p_LT.x = 90;
	//p_LT.y = 50;
	//p_RB.x = 1900;
	//p_RB.y = 1900;
	
	if(calibratedLT && calibratedRB)
	{
		
		cout << "p_LT.x: "<< p_LT.x << endl;
		cout << "p_LT.y: "<< p_LT.y << endl;
		cout << "p_RB.x: "<< p_RB.x << endl;
		cout << "p_RB.y: "<< p_RB.y << endl;
		
		float x_c = (p_LT.x + p_RB.x) / 2;
		float x_d = (p_LT.x - p_RB.x) / 2;
		float y_c = (p_LT.y + p_RB.y) / 2;
		float y_d = (p_LT.y - p_RB.y) / 2;
		
		p_RT.x = x_c - y_d;
		p_RT.y = y_c + x_d;
		p_LB.x = x_c + y_d;
		p_LB.y = y_c - x_d;
		
		cout << "p_LB.x: "<< p_LB.x << endl;
		cout << "p_LB.y: "<< p_LB.y << endl;
		cout << "p_RT.x: "<< p_RT.x << endl;
		cout << "p_RT.y: "<< p_RT.y << endl;
		
		//Min max angle
		Point points[4];
		// points[0] = p_LT;
		// points[1] = p_RT;
		// points[2] = p_LB;
		// points[3] = p_RB;
		// Point p_max = find_min(points, false);
		//Point p_min = find_min(points, true);
		
		// float a_min = p_min.x / p_min.y;
		float a_min = p_LB.x / p_LB.y;
		// float a_max = p_max.x / p_max.y;
		float a_max = p_RT.x / p_RT.y;
		
		// cout << "p_max.y: "<< p_max.y << endl;
		// cout << "p_max.x: "<< p_max.x << endl;
		// cout << "p_min.y: "<< p_min.y << endl;
		// cout << "p_min.x: "<< p_min.x << endl;
		cout << "a_max: "<< a_max << endl;
		cout << "a_min: "<< a_min << endl;

		
		min_angle_degree = (int) floor(atan(a_min) * (180.0/M_PI));
		max_angle_degree = (int) ceil(atan(a_max) * (180.0/M_PI));
		
		cout << "min_angle_degree: "<< min_angle_degree << endl;
		cout << "max_angle_degree: "<< max_angle_degree << endl;
		
		calibration_end = true;
		
	}
	
	return is_point_00 ? calibratedLT : calibratedRB;
}

void  URG_touch_screen::stop_reading_data_from_sensor() {
	work = false;
}

void  URG_touch_screen::set_work(bool work_value) {
	work = work_value;
}

URG_touch_screen::Point URG_touch_screen::find_min(Point* points, bool is_x){
	
	Point min_point;
	min_point.x = numeric_limits<float>::max();
	min_point.y = numeric_limits<float>::max();
	
	if(is_x)
	{
		for(int i=0; i < 4; i++)
		{
			if(points[i].x < min_point.x)
				min_point = points[i];
		}
	}
	else
	{
		for(int i=0; i < 4; i++)
		{
			if(points[i].y < min_point.y)
				min_point = points[i];
		}
	}
	
	return min_point;
}

float URG_touch_screen::area_triangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
    return (float) abs(((float)x1 * (float)(y2 - y3) + (float)x2 * (float)(y3 - y1) + (float)x3 * (float)(y1 - y2)) / 2.0);
}
 
bool URG_touch_screen::is_piont_in_rectangle(Point P1, Point P2, Point P3, Point P4, float x, float y)
{
    float A = area_triangle(P1.x, P1.y, P2.x, P2.y, P3.x, P3.y) + area_triangle(P1.x, P1.y, P4.x, P4.y, P3.x, P3.y);
 
    float A1 = area_triangle(x, y, P1.x, P1.y, P2.x, P2.y);
 
    float A2 = area_triangle(x, y, P2.x, P2.y, P3.x, P3.y);

    float A3 = area_triangle(x, y, P3.x, P3.y, P4.x, P4.y);
 
    float A4 = area_triangle(x, y, P1.x, P1.y, P4.x, P4.y);

	cout << "A=  "<< A << " B= " << A1 + A2 + A3 + A4 << endl;
 
    return (A == A1 + A2 + A3 + A4);
}

float URG_touch_screen::distance_point_from_streight(Point P1, Point P2, float x, float y){
	
	float A = -((P1.y - P2.y) / (P1.x - P2.x));
	float B = 1;
	float C = -(P1.y - ((P1.y - P2.y) / (P1.x - P2.x)) * P1.x);
	
	float distance = (abs(A*x + B*y + C)) / (sqrt(pow(A, 2) + pow(B, 2)));
	
	return distance;
}
