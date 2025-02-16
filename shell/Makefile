
shell: main.c syntax.c syntax.h
	gcc -o shell main.c syntax.c -Wall -Wextra

print_syntax: print_syntax.c syntax.c syntax.h
	gcc -o print_syntax print_syntax.c syntax.c -Wall -Wextra

clean:
	rm -f shell
