
CFLAGS := -Wall -Wextra -Werror
shell: main.c syntax.c syntax.h vartable.h vartable.c
	gcc -o shell main.c syntax.c vartable.c $(CFLAGS)

clean:
	rm -f shell
