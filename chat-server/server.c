#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Required by event.h. */
#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

/* On some systems (OpenBSD/NetBSD/FreeBSD) you could include
 * <sys/queue.h>, but for portability we'll include the local copy. */
#include "queue.h"
#include "constants.h"
#include "std_defs.h"

/* Libevent. */
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

/* The libevent event base.  In libevent 1 you didn't need to worry
 * about this for simple programs, but its used more in the libevent 2
 * API. */
static struct event_base *evbase;

/**
 * The head of our tailq of all connected clients.  This is what will
 * be iterated to send a received message to all connected clients.
 */
TAILQ_HEAD(, client) client_tailq_head;


/**
 * Set a socket to non-blocking mode.
 */
int
setnonblock(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0)
		return flags;
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0)
		return -1;

	return 0;
}

/**
 * Called by libevent when there is data to read.
 */
void
buffered_on_read(struct bufferevent *bev, void *arg)
{
	uint8_t data[8192];
	size_t n;

	n = bufferevent_read(bev, data, sizeof(data));
	if (n <= 0) {
		/* Done. */
		printf("n <=0 in %s",__FUNCTION__);
		return;
	}
	pkt_t * inpkt = (pkt_t *) data;

#undef _
#define _(V,v)											\
	case V:											\
	v##_handler((v##_pkt_t *) &(inpkt->u.v##_pkt),arg);					\
	break;
	
	switch(inpkt->type)
	{
		foreach_server_pkt_type
	}
	//bufferevent_write(client->buf_ev, "order ack!",  n);

}

/**
 * Called by libevent when there is an error on the underlying socket
 * descriptor.
 */
void
buffered_on_error(struct bufferevent *bev, short what, void *arg)
{
	struct client *client = (struct client *)arg;

	if (what & BEV_EVENT_EOF) {
		/* Client disconnected, remove the read event and the
		 * free the client structure. */
		printf("Client disconnected.\n");
	}
	else {
		warn("Client socket error, disconnecting.\n");
	}

	/* Remove the client from the tailq. */
	TAILQ_REMOVE(&client_tailq_head, client, entries);

	bufferevent_free(client->buf_ev);
	close(client->fd);
	free(client);
}

/**
 * This function will be called by libevent when there is a connection
 * ready to be accepted.
 */
void
on_accept(int fd, short ev, void *arg)
{
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	struct client *client;

	client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0) {
		warn("accept failed");
		return;
	}

	/* Set the client socket to non-blocking mode. */
	if (setnonblock(client_fd) < 0)
		warn("failed to set client socket non-blocking");

	/* We've accepted a new client, create a client object. */
	client = calloc(1, sizeof(*client));
	if (client == NULL)
		err(1, "malloc failed");
	client->fd = client_fd;
	client->is_cook = 0;

	client->buf_ev = bufferevent_socket_new(evbase, client_fd, 0);
	bufferevent_setcb(client->buf_ev, buffered_on_read, NULL,
	    buffered_on_error, client);

	/* We have to enable it before our callbacks will be
	 * called. */
	bufferevent_enable(client->buf_ev, EV_READ);

	/* Add the new client to the tailq. */
	TAILQ_INSERT_TAIL(&client_tailq_head, client, entries);

	printf("Accepted connection from %s\n", 
	    inet_ntoa(client_addr.sin_addr));
}

int
main(int argc, char **argv)
{
	int listen_fd;
	struct sockaddr_in listen_addr;
	struct event ev_accept;
	int reuseaddr_on;

	/* Initialize libevent. */
        evbase = event_base_new();

	/* Initialize the tailq. */
	TAILQ_INIT(&client_tailq_head);

	/* Create our listening socket. */
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
		err(1, "listen failed");
	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	listen_addr.sin_port = htons(SERVER_PORT);
	if (bind(listen_fd, (struct sockaddr *)&listen_addr,
		sizeof(listen_addr)) < 0)
		err(1, "bind failed");
	if (listen(listen_fd, 5) < 0)
		err(1, "listen failed");
	reuseaddr_on = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on, 
	    sizeof(reuseaddr_on));

	/* Set the socket to non-blocking, this is essential in event
	 * based programming with libevent. */
	if (setnonblock(listen_fd) < 0)
		err(1, "failed to set server socket to non-blocking");

	/* We now have a listening socket, we create a read event to
	 * be notified when a client connects. */
        event_assign(&ev_accept, evbase, listen_fd, EV_READ|EV_PERSIST, 
	    on_accept, NULL);
	event_add(&ev_accept, NULL);

	/* Start the event loop. */
	event_base_dispatch(evbase);

	return 0;
}