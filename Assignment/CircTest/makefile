EXEC = CircTest
CFLAGS = gcc -Wall -Werror -pedantic -ansi -pthread
all = circularQueue.o ${EXEC}

all: ${all}

circularQueue.o : circularQueue.c circularQueue.h
	${CFLAGS} circularQueue.c -c

${EXEC} : ${EXEC}.c circularQueue.o
	${CFLAGS} circularQueue.o ${EXEC}.c -o ${EXEC}

clean:
	rm -f ${all}
