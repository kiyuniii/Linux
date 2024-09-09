CC = gcc
CFLAGS = -Wall -g

TARGET = $(basename $@).out

all: $(TARGET)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(basename $(TARGET)).o $(basename $(TARGET)).out
