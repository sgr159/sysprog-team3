LIBEVENT = libevent-2.1.8-stable
CFLAGS =	-I$(LIBEVENT)/include -Wall

LIBS =		$(LIBEVENT)/.libs/libevent.a -lrt

all: server cook-client client

server: server.c pkt_handlers.c logging.c thpool.c
	$(CC) $(CFLAGS) -g -pthread -o $@ $^ $(LIBS) -DSERVER=1 -D THPOOL_DEBUG 
client: client.c pkt_handlers.c logging.c
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIBS)
cook-client: cook-client.c pkt_handlers.c logging.c
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIBS)
clean:
	rm -f chat-server cook-client server client *~
