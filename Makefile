TARGET = computer_graphics

ROOTDIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/
SRCDIR = src
OBJDIR = obj
# Debug is the default for Eclipse
BINDIR = Debug

SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/implementations/*.cpp)
INCL := $(wildcard $(SRCDIR)/*.h $(SRCDIR)/implementations/*.hpp)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

CXX = g++
LINKERXX = g++
FLAGS = -Wall -pedantic
CCFLAGS = $(FLAGS)
LFLAGS = $(FLAGS)
rm = rm -vf

NUMJOBS=${NUMJOBS:-" -j7 "}
CCACHE_EXISTS := $(shell ccache -V)
ifdef CCACHE_EXISTS
    CXX := ccache $(CXX)
    LINKERXX := ccache $(LINKERXX)
endif

include Makefile.OS_DETECT

ifdef PNG
	LFLAGS += -lpng
	CCFLAGS += -D PNG
endif

ifdef PRODUCTION
	FLAGS = -O3
else
	FLAGS = -O0 -g3
endif

CCFLAGS += $(FLAGS)
LFLAGS += $(FLAGS)

$(BINDIR)/$(TARGET): $(OBJS)
	@echo "Linking "$<
	@mkdir -p $(dir $@)
	$(LINKERXX) $(OBJS) $(LFLAGS) -o $@ 

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@echo "Compiling "$<
	@mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CCFLAGS)

.PHONEY: clean
clean:
	@$(rm) $(OBJS) $(BINDIR)/$(TARGET)

all: $(BINDIR)/$(TARGET)

run: all
	$(BINDIR)/$(TARGET)

