src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS = -lpthread -ljson-c -lpaho-mqtt3a -lpaho-mqtt3cs

pnet_client: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) pnet_client
