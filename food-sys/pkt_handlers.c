#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <event2/bufferevent.h>

#include "std_defs.h"

#ifdef SERVER
extern TAILQ_HEAD(, client) client_tailq_head;
#endif

PKT_HNDL_FUNC(reg_cook)
{
	struct client * client = (struct client *) arg;
	client->is_cook = 1;
	printf("cook %s %s registered!\n",pkt_data->first_name, pkt_data->last_name);
	return 0;
}

PKT_HNDL_FUNC(reg_customer)
{
	return 0;
}

PKT_HNDL_FUNC(cust_order)
{
#ifdef SERVER
	struct client * client = (struct client *) arg;
	struct client * client_item;
	int cook_found = 0;
	struct pkt_t outpkt;
	TAILQ_FOREACH(client_item, &client_tailq_head, entries) {
		if (client_item->is_cook) {
			cook_found=1;
			client_item->buf_ev_cust = client->buf_ev;
			struct cook_order_pkt_t order_pkt;
			memcpy(order_pkt.orders,pkt_data->orders,sizeof(order_pkt.orders));
			outpkt.type = COOK_ORDER;
			outpkt.size = sizeof(order_pkt);
			outpkt.u.cook_order_pkt = order_pkt;			
			bufferevent_write(client_item->buf_ev, &outpkt,  sizeof(outpkt));
			client_item->is_cook = 0;
			break;
		}
		if(!cook_found) {
			struct cust_update_pkt_t update_pkt;
			strcpy(update_pkt.message,"No Cook available");
			outpkt.type = CUST_UPDATE;
			outpkt.size = sizeof(update_pkt);
			outpkt.u.cust_update_pkt = update_pkt;
			bufferevent_write(client->buf_ev, &outpkt,  sizeof(outpkt));
		}
	}
#endif
	return 0;
}

PKT_HNDL_FUNC(cook_order)
{
	int sockfd = *(int *)(arg);
	struct pkt_t pkt; 
	memset(&pkt,0,sizeof(pkt_t)); 
	printf("orders recieved : ");
	for(int i=0;pkt_data->orders[i]!=0;i++)
	{
		printf("%d ",pkt_data->orders[i]);
	}
	printf("\n Enter completion message: ");
	pkt.type = COOK_UPDATE;
	fgets (pkt.u.cook_update_pkt.message, sizeof(pkt.u.cook_update_pkt.message)-1, stdin);
	pkt.size = sizeof(pkt.u.cook_update_pkt);
	write(sockfd, &pkt, sizeof(pkt_t));
	return 0;
}

PKT_HNDL_FUNC(cust_update)
{
	printf("Message for you: %s\n",pkt_data->message);
	return 0;
}

PKT_HNDL_FUNC(cook_update)
{
	struct client * client = (struct client *) arg;
	struct pkt_t outpkt;
	struct cust_update_pkt_t update_pkt;
	strcpy(update_pkt.message,pkt_data->message);
	outpkt.type = CUST_UPDATE;
	outpkt.size = sizeof(update_pkt);
	outpkt.u.cust_update_pkt = update_pkt;
	bufferevent_write(client->buf_ev_cust, &outpkt,  sizeof(outpkt));
	client->is_cook = 1;
	return 0;
}
