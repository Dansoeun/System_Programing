CC = gcc
OBJS = pushpull_main.o push_to_remote.o pull_from_remote.o copy_file.o compare_files.o
TARGET = pushpull_ipc

all:$(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

pushpull_main.o: pushpullheader.h pushpull_main.c
	$(CC) -c pushpull_main.c

push_to_remote.o: pushpullheader.h push_to_remote.c
	$(CC) -c push_to_remote.c

pull_from_remote.o: pushpullheader.h pull_from_remote.c
	$(CC) -c pull_from_remote.c

copy_file.o: pushpullheader.h copy_file.c
	$(CC) -c copy_file.c

compare_files.o: pushpullheader.h compare_files.c
	$(CC) -c compare_files.c


clean:
	rm $(TARGET)
	rm $(OBJS)
