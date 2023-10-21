srcs := main.c my_func.c
blddir := build
objs := $(addprefix $(blddir)/,$(srcs:.c=.o))
ifeq ($(OS),Windows_NT)
    exe := ./$(blddir)/main.exe
	rm = del /q
else
    exe := ./$(blddir)/main
	rm = rm
endif

CC = g++
CFLAGS =

.PHONY: all clean

all: $(exe)

$(blddir):
	mkdir -p $(@)

$(blddir)/%.o: %.c | $(blddir)
	$(CC) $(CFLAGS) -o $@ -c -Wall -g $<

$(exe) : $(objs)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	$(rm) build/*

run: all
	$(exe)