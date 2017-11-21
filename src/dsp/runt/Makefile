LDFLAGS+=-L$(HOME)/.runt/lib -lrunt -ldl
CFLAGS+=-I$(HOME)/.runt/include -Wall -ansi -pedantic
CFLAGS+=-I../ -I../../ -DEXPORT=""

default: surgeon_parser trinity_parser

trinity_parser: trinity_parser.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)


surgeon_parser: surgeon_parser.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -rf trinity_parser surgeon_parser
