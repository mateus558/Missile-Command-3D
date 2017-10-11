all:
	g++ --std=c++11 -g main.cpp Random.cpp utils.h utils.cpp GameObjects.cpp -o main -lGL -lGLU -lglut 
