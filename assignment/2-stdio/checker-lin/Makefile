build: so_stdio

so_stdio: so_stdio.o utils.o
	gcc -g -shared so_stdio.o utils.o -o libso_stdio.so
	# gcc -g so_stdio.o utils.o -o so_stdio
so_stdio.o: so_stdio.c
	gcc -fPIC -Wall -g   -c so_stdio.c

utils.o: utils.c
	gcc -fPIC -Wall -g   -c utils.c

run:
	./so_stdio $(arg)

clean:
	rm -f *.o
	rm -f libso_stdio.so