all: mem_1 mem_2 mem_3
mem_1: mem_1.o memlayout.o
	gcc mem_1.o memlayout.o -m32 -o mem_1

mem_2: mem_2.o memlayout.o
	gcc mem_2.o memlayout.o -m32 -o mem_2

mem_3: mem_3.o memlayout.o
	gcc mem_3.o memlayout.o -m32 -o mem_3


memlayout.o: memlayout.c
	gcc $(CFLAGS) memlayout.c -c  -m32

mem_1.o: mem_1.c
	gcc mem_1.c -c -m32

mem_2.o: mem_2.c
	gcc mem_2.c -c -m32

mem_3.o: mem_3.c
	gcc mem_3.c -c -m32

clean:
	rm -f *.o mem_1 mem_2 mem_3
