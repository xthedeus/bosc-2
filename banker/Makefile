all: banker

OBJS = banker.o
LIBS= -lm -lpthread -pthread

banker: banker.o ${OBJS}
	gcc -o $@ ${OBJS} ${LIBS}

clean:
	rm -rf *o banker