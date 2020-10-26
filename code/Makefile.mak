
ovc: main.o stash.o status.o reset.o unselect.o select.o log.o commit.o init.o help.o cf.o diff.o
	gcc main.o stash.o status.o reset.o unselect.o select.o log.o commit.o init.o help.o cf.o diff.o -o ovc

main.o: main.c main.h
	gcc -c main.c

stash.o: stash.c main.h
	gcc -c stash.c

status.o: status.c main.h
	gcc -c status.c

reset.o: reset.c main.h
	gcc -c reset.c

unselect.o: unselect.c main.h
	gcc -c unselect.c

select.o: select.c main.h
	gcc -c select.c

log.o: log.c main.h
	gcc -c log.c

commit.o: commit.c main.h
	gcc -c commit.c

init.o: init.c main.h
	gcc -c init.c

help.o: help.c main.h
	gcc -c help.c

cf.o: cf.c main.h
	gcc -c cf.c

diff.o: diff.c main.h
	gcc -c diff.c

clean:
	del *.o

target: dependencies
	action