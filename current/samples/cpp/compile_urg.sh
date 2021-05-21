#!/usr/bin/bash
echo compile URG
rm -rf urg_server
rm -rf URG_touch_screen.o
g++ -c -I../../include/cpp -lcpr -lcrypto -lpthread -lboost_system -lcpprest -L../../src -lurg_cpp URG_touch_screen.cpp
g++ -o urg_server -I../../include/cpp -lcpr -lcrypto -lpthread -lboost_system -lcpprest -L../../src -lurg_cpp *.o
export LD_LIBRARY_PATH=/home/pi/urg_touch_screen/current/src/
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH":/usr/local/lib/