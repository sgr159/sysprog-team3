#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <stdint.h> 

#include "constants.h"
#include "std_defs.h"

int main(int argc, char *argv[])
{
    int sockfd = 0;
    struct sockaddr_in serv_addr; 

    if(argc != 2)
    {
        printf("\n Usage: %s <ip of server>\n",argv[0]);
        return 1;
    } 

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT); 

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    }
    printf("WELCOME! Please select your order\n");
    int max_num_orders=1;
/*
 * print the menu
 */

#undef _
#define _(V)					\
    printf("%d : "#V,max_num_orders++);			\
    printf("\n");

    foreach_order_type
    
    char order[64];
    char order_tok[64];
    char *tok;
    cust_order_pkt_t order_data;
    printf("Enter the numbers next to your choice separated by spaces: ");
    int i = 0;
    int retval = 0;

renter_order:
    memset(&order_data,0,sizeof(cust_order_pkt_t));
    memset(order,'\0',sizeof(order));
    fgets (order, sizeof(order)-1, stdin);
    printf("order :%s\n",order);
    strcpy(order_tok,order);
    tok = strtok(order_tok," ");
    i=0;
    while(tok!=NULL)
    {
	    int order = atoi(tok);
	    if(order == 0 || order >= max_num_orders)
	    {
		    printf ("Invalid order %s, \nrenter your order:",tok);
		    goto renter_order;
	    }
	    order_data.orders[i++] = (uint8_t) order;
	    tok=strtok(NULL," ");
    }

    // send the order
    pkt_t pkt;
    pkt.type = CUST_ORDER;
    pkt.size = sizeof(order_data);
    pkt.u.cust_order_pkt = order_data;
    write(sockfd, &pkt, sizeof(pkt_t));
    memset(&pkt,0,sizeof(pkt_t));

read:
    read(sockfd,&pkt,sizeof(pkt_t));

#undef _
#define _(V,v)											\
	case V:											\
	retval = v##_handler((v##_pkt_t *) &(pkt.u.v##_pkt),NULL);					\
	break;
	
	switch(pkt.type)
	{
		foreach_client_pkt_type
	}
	if(retval)
		goto read;
   
    return 0;
}
