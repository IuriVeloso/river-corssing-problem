CC = gcc

CFLAGS  = -Wall -Werror -D_POSIX_THREAD_SEMANTICS
LDFLAGS = -pthread

# the build target executable:
TARGET = main

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(LDFLAGS)

clean:
	rm $(TARGET)
