
CFLAGS := -Wall -Wextra -Werror
shell: main.c syntax.c syntax.h
	gcc -o shell main.c syntax.c $(CFLAGS)

print_syntax: print_syntax.c syntax.c syntax.h
	gcc -o print_syntax print_syntax.c syntax.c -Wall -Wextra

clean:
	rm -f shell
