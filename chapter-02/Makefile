
DEBUG = -g -I/usr/include/SDL2
RELEASE = -O3 -I/usr/include/SDL2

all:
	g++ ${DEBUG} -std=c++11 main.cpp -o main -lSDL2 -lGL -lGLU -lSDL2_image

clean:
	rm -f main
	rm -f *.o
