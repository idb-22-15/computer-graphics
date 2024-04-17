# nix-shell -p mesa libGL libGLU freeglut
#make clean && g++ main.cpp -o main -lGL -lglut -lGLU -std=c++2a && ./main
all:
	g++ main1.cpp -o main -lGL -lGLU -lglut -lX11 -lXrandr -std=c++2a

glfw:
	g++ main-glfw.cpp -o main -lGL -lGLU -lglfw -lX11 -lXrandr -std=c++2a && ./main

clean:
	rm -f main