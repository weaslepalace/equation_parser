################################################################
#					Makefile
################################################################

TARGET = test

INCLUDE_PATH =
LIB_PATH =

OBJDIR = obj
MKOBJDIR := $(shell if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi)
INCDIR = inc
SOURCES := $(wildcard *.c)
INCLUDES := $(wildcard $(INCDIR)/*.h)
OBJECTS := $(patsubst %.c, $(OBJDIR)/%.o, $(SOURCES))


CC      = gcc
CFLAGS  = -g -std=c99 -D_GNU_SOURCE -Wall
IFLAGS 	:= -I$(INCDIR)
LDFLAGS := -L$(LIB_PATH)  -Wl,-rpath=$(LIB_PATH)
LDLIBS 	= -lm 

.SUFFIXES:
.PHONY: all
all: $(TARGET)

$(OBJDIR)/%.o: %.c $(INCLUDES)
	$(CC) $(IFLAGS) -c $(CFLAGS) -o $@ $<

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	
.PHONY: clean
clean:
	$(RM) $(OBJECTS) $(TARGET_PATH)
	$(RM) -r $(OBJDIR)
