CC = g++
CFLAGS = -Isrc -O3 -Wall -std=c++11
rm = @rm
mkdir = @mkdir
exe = mapper
OBJs = objs/QASMparser.o \
		objs/QASMscanner.o \
		objs/QASMtoken.o
HPPs =  src/GateNode.hpp \
		src/QASMparser.h \
		src/util.cpp

ifeq ($(OS),Windows_NT)
	rm = @del /F /Q
	CFLAGS += -D WINDOWS
	exe = mapper.exe
else
	mkdir += -p 
	rm += -f -r 
endif

default: objs ${exe}

debug: CFLAGS += -g -O0
debug: default
prof: CFLAGS += -pg -fno-inline
prof: default


mapper: src/main.cpp ${OBJs} ${HPPs}
	${CC} ${CFLAGS} ${OBJs} $< -o $@

mapper.exe: src/main.cpp ${OBJs} ${HPPs}
	${CC} ${CFLAGS} ${OBJs} $< -o $@

objs:
	${mkdir} objs

objs/QASMparser.o: src/QASMparser.cpp src/QASMparser.h
	${CC} ${CFLAGS} -c $< -o $@

objs/QASMscanner.o: src/QASMscanner.cpp src/QASMscanner.hpp
	${CC} ${CFLAGS} -c $< -o $@

objs/QASMtoken.o: src/QASMtoken.cpp src/QASMtoken.hpp
	${CC} ${CFLAGS} -c $< -o $@


clean:
	${rm} mapper mapper.exe objs
