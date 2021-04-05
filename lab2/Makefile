src = $(wildcard *.c)
head = $(wildcard *.h)
obj = $(src:.c=.o)

tsig: $(obj) $(head)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	
%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f *.o
	rm -f tsig
