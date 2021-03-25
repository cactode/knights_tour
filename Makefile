all: tour.cpp
	mkdir -p build
	clang++ tour.cpp -o build/tour -Wall -std=c++17 -O3

run: all
	./tour