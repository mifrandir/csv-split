csv-split: src/main.c src/cli.c
	$(CC) $(CFGLAGS) $? $(LDFLAGS) -o $@