#!/usr/bin/bash
echo start urg_server
rm -rf urg_server
rm -rf *.o
g++ -c -I../../include/cpp -lcpr -lcrypto -lpthread -lboost_system -lcpprest -L../../src -lurg_cpp *.cpp
g++ -o urg_server -I../../include/cpp -lcpr -lcrypto -lpthread -lboost_system -lcpprest -L../../src -lurg_cpp *.o
export LD_LIBRARY_PATH=/home/pi/Desktop/Test2/urg_touch_screen/current/src/
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH":/usr/local/lib/