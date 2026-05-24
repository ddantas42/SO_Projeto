CC = gcc
LIBFLAGS = -Isrc/lib
CFLAGS = -Wall -Wextra -Werror -pthread $(LIBFLAGS)
TARGET = drone

SRCDIR = src/
SRCFILES = log.c sample.c init_shm.c analysis_process.c main.c 

SRCS = $(addprefix $(SRCDIR), $(SRCFILES)) 
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

re: clean all

.PHONY: all clean re
