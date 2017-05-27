LIBEVENT = libevent-2.1.8-stable
CFLAGS =	-I$(LIBEVENT)/include -Wall

LIBS =		$(LIBEVENT)/.libs/libevent.a -lrt

all: server cook-client client

server: server.c pkt_handlers.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -DSERVER=1

chat-server: chat-server.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
client: client.c pkt_handlers.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
cook-client: cook-client.c pkt_handlers.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
clean:
	rm -f chat-server cook-client server client *~
