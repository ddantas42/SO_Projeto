CC = gcc
CFLAGS = -Wall -Wextra -Werror
TARGET = drone

SRCDIR = src/
SRCFILES = main.c

SRCS = $(addprefix $(SRCDIR), $(SRCFILES)) 
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
