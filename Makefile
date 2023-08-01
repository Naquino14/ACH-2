program: program.o ACH-2.o
	gcc -o program program.o ACH-2.o -ggdb -Wall -Wextra -pedantic -std=c99 -Wno-unknown-pragmas

program.o: program.c ACH-2.c
	gcc -c program.c -ggdb -Wall -Wextra -pedantic -std=c99 -Wno-unknown-pragmas

ACH-2.o: ACH-2.c
	gcc -c ACH-2.c -ggdb -Wall -Wextra -pedantic -std=c99 -Wno-unknown-pragmas

clean:
	rm -f *.o program