CFLAGS = -g
#-o2 -g

all: folders server client

server: bin/monitor

client: bin/tracer

folders:
	@mkdir -p src obj bin tmp

bin/monitor: obj/monitor.o
	gcc ${CFLAGS} obj/monitor.o -o bin/monitor

obj/monitor.o: src/monitor.c
	gcc -Wall ${CFLAGS} -c src/monitor.c -o obj/monitor.o

bin/tracer: obj/tracer.o
	gcc ${CFLAGS} obj/tracer.o -o bin/tracer

obj/tracer.o: src/tracer.c
	gcc -Wall ${CFLAGS} -c src/tracer.c -o obj/tracer.o

bin/queue: obj/queue.o
	gcc ${CFLAGS} obj/queue.o -o bin/queue

obj/tracer.o: src/queue.c
	gcc -Wall ${CFLAGS} -c src/queue.c -o obj/queue.o


clean:
	rm -f obj/* tmp/* bin/{tracer,monitor}
