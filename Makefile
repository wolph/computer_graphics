CC = g++
SRC = src/main.cpp src/mesh.cpp src/raytracing.cpp
LIBS = -lGL -lGLU -lglut
INCLUDE = -I.
EXEC = Default/computer_graphics

# Source: http://stackoverflow.com/questions/714100/os-detecting-makefile
ifeq ($(OS),Windows_NT)
    CCFLAGS += -D WIN32
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        CCFLAGS += -D AMD64
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        CCFLAGS += -D IA32
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CCFLAGS += -D LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        CCFLAGS += -D OSX
		LIBS = -framework OpenGL -framework GLUT
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CCFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CCFLAGS += -D ARM
    endif
endif


all:
	mkdir -p Default
	$(CC) $(SRC) -o $(EXEC) $(LIBS) $(INCLUDE)

run: all
	./$(EXEC)

clean:
	rm -rf $(EXEC) *~
