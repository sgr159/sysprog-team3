#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <event2/bufferevent.h>

#include "std_defs.h"
#include "logging.h"

#ifdef SERVER
extern TAILQ_HEAD(, client) client_tailq_head;
#endif

PKT_HNDL_FUNC(reg_cook)
{
	struct client * client = (struct client *) arg;
	client->state = COOK_FREE;
	//printf("cook %s %s registered!\n",pkt_data->first_name, pkt_data->last_name);
	for(int i=0;i<MENU_ARR_SIZE;i++)
	{
		if(pkt_data->capability[i] == CAPABLE){
			client->menu_items[i] = pkt_data->capability[i];
			LOG(LOG_DEBUG,"capability added %d",i)
		}
	}

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
	if(UNKNOWN == client->state)
		client->state = CUSTOMER;
	if(CUSTOMER != client->state)
	{
		LOG(LOG_WARNING,"%s","order received from someone not a customer")
		return 1;

	}
	client->num_of_orders = 0;
	for(int i=0;i<MENU_ARR_SIZE && pkt_data->orders[i]!=UNKNOWN;i++)
	{
		client->menu_items[pkt_data->orders[i]] = NEW;
		client->num_of_orders++;
		LOG(LOG_DEBUG,"received order %d",pkt_data->orders[i])

	}
	struct client * client_item;
	int orders_assigned = 0;
	struct pkt_t outpkt;
	TAILQ_FOREACH(client_item, &client_tailq_head, entries) {
		if (client_item->state == COOK_FREE) {
			client_item->assigned_client = client;
			struct cook_order_pkt_t order_pkt;
			memset(order_pkt.orders,UNKNOWN,sizeof(order_pkt.orders));
			int prev_orders_taken = orders_assigned;
			for(int i=0;i<MENU_ARR_SIZE;i++)
			{
				if(NEW == client->menu_items[i] && CAPABLE == client_item->menu_items[i])
				{
					order_pkt.orders[i] = client->menu_items[i] = client_item->menu_items[i] = ASSIGNED;
					orders_assigned++;
				}
			}

			if(prev_orders_taken == orders_assigned)
				continue;
				
			outpkt.type = COOK_ORDER;
			outpkt.size = sizeof(order_pkt);
			outpkt.u.cook_order_pkt = order_pkt;			
			bufferevent_write(client_item->buf_ev, &outpkt,  sizeof(outpkt));
			client_item->state = COOK_BUSY;
			if(client->num_of_orders == orders_assigned)
				break;
		}
	}
	if(!(client->num_of_orders == orders_assigned)) {
		char customer_msg[64];
		if(orders_assigned == 0)
		{
			snprintf(customer_msg,sizeof(customer_msg),ORDER_UNSERV_MESSAGE);
		}
		else
		{
			char missed_orders[64] = "";
			char missed_order[5];
			for(int i=0;i<MENU_ARR_SIZE;i++)
			{
				if(client->menu_items[i] == NEW){
					snprintf(missed_order,sizeof(missed_order),"%d ",i);
					strcat(missed_orders,missed_order);
				}
			}
			snprintf(customer_msg,sizeof(customer_msg),
					"Orders unserviceable currently %s, rest of the orders will be serviced",missed_orders);
		}
		
		LOG(LOG_INFO,"%s",customer_msg)
		struct cust_update_pkt_t update_pkt;
		strncpy(update_pkt.message,customer_msg,sizeof(update_pkt.message));
		outpkt.type = CUST_UPDATE;
		outpkt.size = sizeof(update_pkt);
		outpkt.u.cust_update_pkt = update_pkt;
		bufferevent_write(client->buf_ev, &outpkt,  sizeof(outpkt));
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
	for(int i=0;i<MENU_ARR_SIZE;i++)
	{
		if(pkt_data->orders[i] == ASSIGNED)
			printf("%d ",i);
	}
	printf("\n press enter when your orders are ready.");
	pkt.type = COOK_UPDATE;
	fgets (pkt.u.cook_update_pkt.message, sizeof(pkt.u.cook_update_pkt.message)-1, stdin);
	pkt.size = sizeof(pkt.u.cook_update_pkt);
	write(sockfd, &pkt, sizeof(pkt_t));
	return 0;
}

PKT_HNDL_FUNC(cust_update)
{
	printf("Message for you: %s\n",pkt_data->message);
	if(strcmp(pkt_data->message,ORDER_COMPLETE_MESSAGE)==0)
		return 0;
	if(strcmp(pkt_data->message,ORDER_UNSERV_MESSAGE)==0)
		return 0;
	return 1;
}

PKT_HNDL_FUNC(cook_update)
{
#ifdef SERVER
	struct client * client = (struct client *) arg;
	struct client * client_item;
	struct pkt_t outpkt;
	struct cust_update_pkt_t update_pkt;
	int customer_exists = 0;
	TAILQ_FOREACH(client_item, &client_tailq_head, entries) {
		if(client_item == client->assigned_client){
			customer_exists = 1;
			break;
		}
	}
	if(!customer_exists)
	{
		LOG(LOG_ERR,"%s","attempt to give an update to a non existing customer")
		return 1;
	}
	for(int i=0;i<MENU_ARR_SIZE;i++)
	{
		if(client->menu_items[i] == ASSIGNED)
			client->assigned_client->menu_items[i] = COMPLETED; 
	}

	int assigned_found = 0;

	for(int i=0;i<MENU_ARR_SIZE;i++)
	{
		if(client->assigned_client->menu_items[i] == ASSIGNED){
			assigned_found = 1;
			break;
		}
	}
	if (!assigned_found) {
		strcpy(update_pkt.message,ORDER_COMPLETE_MESSAGE);
		outpkt.type = CUST_UPDATE;
		outpkt.size = sizeof(update_pkt);
		outpkt.u.cust_update_pkt = update_pkt;
		bufferevent_write(client->assigned_client->buf_ev, &outpkt,  sizeof(outpkt));
		//TODO: delete client
	}
	client->state = COOK_FREE;
#endif
	return 0;
}
