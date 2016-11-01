CC = gcc -L/usr/local/lib -L/usr/local/include -lIPC
CFLAGS = -Wall
SOURCES = QueueWatcher.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = QueueWatcher

$(EXECUTABLE):	$(OBJECTS)

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
