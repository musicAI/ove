CPPFLAGS=-I/usr/local/opt/qt/include -I/usr/local/opt/qt/include/QtCore #
LDFLAGS=-L/usr/local/opt/qt/lib -F/usr/local/opt/qt/lib #
CC=g++ -std=c++0x -w #
OBJECTS=OveElement.o OveScore.o Block.o Chunk.o ChunkParse.o OveOrganizer.o OveSerialize.o ove.o test.o #

test: $(OBJECTS) importove.o
	$(CC) $(LDFLAGS) -lfreetype -framework QtCore -framework QtWidgets -framework QtQuick -framework QtQml -framework QtSvg -framework QtGui $(OBJECTS) -o $@

%.o: %.cpp
	$(CC) $(CPPFLAGS) -I./thirdparty -I./ -c $< -o $@

lib:
	mkdir -p build && cd build && $(CC) $(CPPFLAGS) -I../ -I../thirdparty -include "globals.h" -c ../libmscore/*.cpp

.PHONY: clean run
clean:
	-rm *.o test
	-rm build/*.o

run:
	-./test sample1.ove
