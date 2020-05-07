EXEC = lift_sim_A
CFLAGS = gcc -Wall -Werror -pedantic -ansi -g -pthread
all = circularQueue.o ${EXEC}

all: ${all}

circularQueue.o : circularQueue.c circularQueue.h
	${CFLAGS} circularQueue.c -c

${EXEC} : ${EXEC}.c ${EXEC}.h  circularQueue.o
	${CFLAGS} circularQueue.o ${EXEC}.c -o ${EXEC}

clean:
	rm -f ${all}
