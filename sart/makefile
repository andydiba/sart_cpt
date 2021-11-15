CC = gcc
CFLAGS = -Wall
TARGET = web
OBJFILES = server.o msghandler.o utilities.o 

all: $(TARGET)

$(TARGET): $(OBJFILES) 
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES)

server.o: server.c msghandler.h
	$(CC) $(CFLAGS) -c server.c

msghandler.o: msghandler.c msghandler.h utilities.h
	$(CC) $(CFLAGS) -c msghandler.c
	

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

clean:
	rm $(TARGET) $(OBJFILES)
