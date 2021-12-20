notapp: main.o server.o observer.o user.o
	gcc main.o server.o observer.o user.o -o notapp -pthread

notapp.time: main.o server_timestamp.o observer.o user.o
	gcc main.o server_timestamp.o observer.o user.o -o notapp.time -pthread

main.o: main.c
	gcc -c main.c

server.o: server.c
	gcc -c server.c 

server_timestamp.o: server.c
	gcc -c server.c -o server_timestamp.o -DTIMESTAMP=1

observer.o: observer.c
	gcc -c observer.c

user.o: user.c
	gcc -c user.c

clean:
	rm -f *.o main server observer user