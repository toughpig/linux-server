CC=gcc
RM=rm
CFLAGS=-I.
DEPS = socket.hpp

SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp,%.o,${SRC})

${OBJ}:${SRC}
	gcc -c ${SRC}

.PHONY: all clean
all: server client

server:${OBJ}
	${CC} server.o ${CFLAGS} -o $@ >/dev/null

client:${OBJ}
	${CC} client.o ${CFLAGS} -o $@ >/dev/null

clean:
	@echo "[cleaning ${TARGET}]"
	${RM} -rf ${OBJ}
	${RM} -rf client server