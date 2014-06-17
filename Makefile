TARGET = computer_graphics

ROOTDIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/
SRCDIR = src
OBJDIR = obj
# Debug is the default for Eclipse, so let's not use it... Eclipse keeps
# overwriting stuff
BINDIR = bin/

SRCS := $(wildcard $(SRCDIR)/*.cpp)
INCL := $(wildcard $(SRCDIR)/*.hpp)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

CXX = g++
LINKERXX = g++
CCFLAGS = -Wall -pedantic -std=c++0x
LFLAGS = -Wall -pedantic -std=c++0x
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
	CCFLAGS += -D IMAGE_FORMAT=0
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

cube: all
	$(BINDIR)/$(TARGET) -m0

simple_monkey: all
	$(BINDIR)/$(TARGET) -m1

monkey: all
	$(BINDIR)/$(TARGET) -m2

dodge: all
	$(BINDIR)/$(TARGET) -m3

trace_cube: all
	$(BINDIR)/$(TARGET) -m0 -r

trace_simple_monkey: all
	$(BINDIR)/$(TARGET) -m1 -r

trace_monkey: all
	$(BINDIR)/$(TARGET) -m2 -r

trace_dodge: all
	$(BINDIR)/$(TARGET) -m3 -r

