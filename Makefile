clean:
	/bin/rm -f main *.o *.gz

debug:
	gdb ./main

test:
	gcc tests.c -o tests && ./tests
