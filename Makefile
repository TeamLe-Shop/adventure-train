CC=clang
CFLAGS=-Wunused-parameters
SOURCES=src/*c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=gatchan

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE)

.c.o:
	echo "Compiling "$<$" -> "$@
	$(CC) $(CFLAGS) $< -o $@
