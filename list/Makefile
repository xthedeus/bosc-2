all: fifo

OBJS = list.o main.o 
LIBS= -lm -lpthread -pthread

fifo: main.o ${OBJS}
	gcc -o $@ ${OBJS} ${LIBS}

clean:
	rm -rf *o fifo
