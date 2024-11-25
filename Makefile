CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = app.out

SRCS = clone.c absolute_path.c directory_check.c file_copy.c file_check.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

clean:
	rm -f $(TARGET)
