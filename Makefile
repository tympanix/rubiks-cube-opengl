all: build

build:
	g++ -Wall -std=c++11 main.cc -o rubiks -framework OpenGl -lglfw