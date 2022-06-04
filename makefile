all: client server test

zip:
	zip archive.zip makefile *.c *.h *.md .gitignore

server: server.o 
	gcc -o server server.o -pthread

test: test.o server client
	gcc -o test test.o
	

client: client.o
	gcc -o client client.o

%.o: %.c
	gcc -c -g $^

clean:
	rm -rf *.o *.zip
	rm server client test