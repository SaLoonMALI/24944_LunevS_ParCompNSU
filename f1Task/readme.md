float - -0.0277862 double - 4.89582e-11

float build:
mkdir build && cd build && cmake -DDOUBLE_OPT=OFF .. && make

double build:
mkdir build && cd build && cmake -DDOUBLE_OPT=ON .. && make
