EXEC = lift_sim_B
CFLAGS = gcc -Wall -Werror -pedantic -ansi -g -lpthread
all = circularQueue.o ${EXEC}

ifdef DEBUG_R
CFLAGS += -D DEBUG_R
DEBUG_R : clean $(EXEC)
endif

ifdef DEBUG_L
CFLAGS += -D DEBUG_L
DEBUG_L : clean $(EXEC)
endif

all: ${all}

circularQueue.o : circularQueue.c circularQueue.h
	${CFLAGS} circularQueue.c -c

${EXEC} : ${EXEC}.c ${EXEC}.h  circularQueue.o
	${CFLAGS} circularQueue.o ${EXEC}.c -o ${EXEC} -lrt -lpthread -pthread

clean:
	rm -f ${all}
