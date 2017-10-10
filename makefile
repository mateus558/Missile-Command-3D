all:
	g++ --std=c++11 utils.h utils.cpp main.cpp Random.cpp GameObjects.cpp -o main -lGL -lGLU -lglut 
