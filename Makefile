CC=gcc
OBJS=checkout.o cp.o process.o
TARGET=app.out

all:$(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

checkout.o: cp.h checkout.c 
	$(CC) -c checkout.c 

cp.o: cp.h cp.c 
	$(CC) -c cp.c 

process.o: process.h process.c
	$(CC) -c process.c

clean:
	rm $(OBJS)
	rm $(TARGET)