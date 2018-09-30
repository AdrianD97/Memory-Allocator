build: allocator

allocator: allocator.o
	gcc allocator.o -o allocator

allocator.o: allocator.c
	gcc -Wall -c allocator.c -o allocator.o

clean:
	rm -f *~ *.o allocator
