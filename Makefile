CC = g++
SRC = src/*.cpp src/implementations/*.cpp
LIBS = -lGL -lGLU -lglut
INCLUDE = -I.

OUTDIR = Debug/
EXEC = $(OUTDIR)computer_graphics
CCFLAGS = -O0 -g3

ifndef BASEDIR
	BASEDIR = ./
endif

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
	CCFLAGS	+= -L /usr/lib/nvidia-319
	CCFLAGS += -L /usr/lib/nvidia-331
    endif
    ifeq ($(UNAME_S),Darwin)
        CCFLAGS += -D OSX -D PNG -D GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
		LIBS = -framework OpenGL -framework GLUT -lpng
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
	cd $(BASEDIR) && \
	mkdir -p Debug && \
	$(CC) $(SRC) -o $(EXEC) $(CCFLAGS) $(LIBS) $(INCLUDE)

run: all
	cd $(BASEDIR) && \
	./$(EXEC)

clean:
	cd $(BASEDIR) && \
	rm -rf $(OUTDIR) $(EXEC) *~

