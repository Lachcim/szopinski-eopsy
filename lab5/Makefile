src = $(wildcard *.c)
head = $(wildcard *.h)
obj = $(src:.c=.o)

philo: $(obj) $(head)
	$(CC) $(CFLAGS) $(LDFLAGS) -pthread $^ -o $@
	
%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f *.o
	rm -f philo
