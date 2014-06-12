TARGET = computer_graphics

ROOTDIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/
SRCDIR = src
OBJDIR = obj
# Debug is the default for Eclipse
BINDIR = Debug

SRCS := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/implementations/*.cpp)
INCL := $(wildcard $(SRCDIR)/*.h $(SRCDIR)/implementations/*.h)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

CC = g++
LINKER = g++ -o
FLAGS = -O0 -g3 -Wall -pedantic
CCFLAGS = $(FLAGS)
LFLAGS = $(FLAGS)
rm = rm -vf

include Makefile.OS_DETECT

ifdef PNG
	LFLAGS += -lpng
	CCFLAGS += -D PNG
endif

$(BINDIR)/$(TARGET): $(OBJS)
	@echo "Linking "$<
	$(LINKER) $@ $(LFLAGS) $(OBJS)

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling "$<
	$(CC) $(CCFLAGS) -c $< -o $@

.PHONEY: clean
clean:
	@$(rm) $(OBJS) $(BINDIR)/$(TARGET)

run: $(BINDIR)/$(TARGET)
	$(BINDIR)/$(TARGET)

