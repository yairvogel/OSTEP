
CFLAGS := -Wall -Wextra -Werror
shell: main.c syntax.c syntax.h vartable.h vartable.c
	gcc -o shell main.c syntax.c vartable.c $(CFLAGS)

print_syntax: print_syntax.c syntax.c syntax.h
	gcc -o print_syntax print_syntax.c syntax.c -Wall -Wextra

clean:
	rm -f shell
