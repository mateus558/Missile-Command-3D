all:
	g++ --std=c++11 -g main.cpp Random.cpp utils.h camera.h shader.cpp transform.h utils.cpp glcTexture.cpp GameObjects.cpp 3DPlyModel.cpp skybox.cpp -o main -lpng -lGL -lGLU -lglut -lGLEW -lalut
