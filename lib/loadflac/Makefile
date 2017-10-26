OBJ= loadflac.o ex_loadflac.o
CFLAGS += -g
LDFLAGS += -lm -lsoundpipe -lsndfile

default: ex_loadflac

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

ex_loadflac: $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJ) ex_loadflac
