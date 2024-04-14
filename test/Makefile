# nix-shell -p mesa libGL libGLU freeglut
#make clean && g++ main.cpp -o main -lGL -lglut -lGLU -std=c++2a && ./main
all:
	g++ main1.cpp -o main -lgl -lglu -lglut -lx11 -lxrandr -std=c++2a

glfw:
	g++ main-glfw.cpp -o main -lgl -lglu -lglfw -lx11 -lxrandr -std=c++2a

clean:
	rm -f main