LIBEVENT = libevent-2.1.8-stable
CFLAGS =	-I$(LIBEVENT)/include -Wall

LIBS =		$(LIBEVENT)/.libs/libevent.a -lrt

chat-server: chat-server.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f chat-server *~
