all:
	g++ --std=c++11 -g main.cpp Random.cpp utils.h utils.cpp glcTexture.cpp GameObjects.cpp 3DPlyModel.cpp -o main -lpng -lGL -lGLU -lglut 
