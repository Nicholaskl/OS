CC = gcc
EXEC = lift_sim_A
CFLAGS = -Wall -Werror -pedantic -ansi -pthread
OBJ = lift_sim_A.o circularQueue.o

ifdef ALL_STATS
CFLAGS += -D ALL_STATS
ALL_STATS : clean $(EXEC)
endif

$(EXEC) : $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

lift_sim_A.o : lift_sim_A.c circularQueue.h lift_sim_A.h
	$(CC) $(CFLAGS) -c lift_sim_A.c 

circularQueue.o : circularQueue.c circularQueue.h
	$(CC) -c circularQueue.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)
