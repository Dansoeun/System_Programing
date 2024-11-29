CC = gcc
CFLAGS = -Wall
OBJS = main.o add.o checkout.o cp.o pushpull_main.o push_to_remote.o pull_from_remote.o copy_file.o compare_files.o branch.o CreateBranch.o process.o clone.o absolute_path.o directory_check.o file_copy.o file_check.o
TARGET = app.out

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

#main 
main.o: main.c add.h add.c checkout.h checkout.c pushpullheader.h pushpull_main.c branch.h branch.c clone.h clone.c
	$(CC) -c main.c

#add 부분 
add.o: add.c add.h
	$(CC) -c add.c

# checkout 부분
checkout.o: cp.h checkout.c 
	$(CC) -c checkout.c 

cp.o: cp.h cp.c 
	$(CC) -c cp.c 

# pushpull
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

# branch
branch.o: CreateBranch.h process.h branch.c
	$(CC) -c branch.c

CreateBranch.o: CreateBranch.h process.h CreateBranch.c
	$(CC) -c CreateBranch.c

process.o: process.h process.c
	$(CC) -c process.c

# clone
clone.o: clone.c
	$(CC) -c clone.c

absolute_path.o: absolute_path.h absolute_path.c 
	$(CC) -c absolute_path.c

directory_check.o: directory_check.h directory_check.c
	$(CC) -c directory_check.c

file_copy.o: file_copy.h file_copy.c 
	$(CC) -c file_copy.c

file_check.o: file_check.h file_check.c
	$(CC) -c file_check.c

# clean rule
clean:
	rm $(OBJS)
	rm $(TARGET)
