float - -9930.62 double - -3.20841e-07

float build:
mkdir build && cd build && cmake -DDOUBLE_OPT=OFF .. && make

double build:
mkdir build && cd build && cmake -DDOUBLE_OPT=ON .. && make
