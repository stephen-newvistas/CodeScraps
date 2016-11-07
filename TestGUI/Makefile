CC = gcc -L/usr/local/lib -L/usr/local/include -lIPC
CFLAGS = -Wall
SOURCES = TestGUI.c ../../Libraries/EscapeSerial/escapeSerial.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = TestGUI

$(EXECUTABLE):	$(OBJECTS)

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
