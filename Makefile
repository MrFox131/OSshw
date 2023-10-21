srcs := main.c my_func.c
blddir := build
objs := $(addprefix $(blddir)/,$(srcs:.c=.o))
ifeq ($(OS),Windows_NT)
    exe := ./$(blddir)/main.exe
else
    exe := ./$(blddir)/main
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
	rm -rf build/*

run: all
	$(exe)