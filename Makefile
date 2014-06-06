CC = g++
SRC = main.cpp mesh.cpp raytracing.cpp
LIBS = -lGL -lGLU -lglut
INCLUDE = -I.
EXEC = raytracer

all:
	$(CC) $(SRC) -o $(EXEC) $(LIBS) $(INCLUDE)

clean:
	rm -rf $(EXEC) *~
