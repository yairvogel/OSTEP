CC = gcc
CFLAGS = -Wall -Wextra -pthread

TARGET = ./mmap
SRC = main.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	$(TARGET)

.PHONY: clean
