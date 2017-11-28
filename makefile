all:
	g++ --std=c++11 -g main.cpp Random.cpp utils.h utils.cpp glcTexture.cpp GameObjects.cpp 3DPlyModel.cpp skybox.cpp glcSound.cpp -o main -lopenal -lalut -lpng -lGL -lGLU -lglut -lGLEW 
