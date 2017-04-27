all: server client

server: server.c group_list.c
	$(CC) $? -o $@
client: client.c
	$(CC) $? -o $@

clean:
	-rm server client
