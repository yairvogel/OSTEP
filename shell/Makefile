
CFLAGS := -Wall -Wextra -Werror
LDFLAGS := -lreadline -lhistory -ltinfo

shell: main.c syntax.c vartable.c
	gcc -o shell main.c syntax.c vartable.c $(LDFLAGS) $(CFLAGS)

clean:
	rm -f shell
