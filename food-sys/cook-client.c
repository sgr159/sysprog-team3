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
#include <assert.h> 

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
    // send the registration packet
    reg_cook_pkt_t reg_data;
/*    printf("Enter first name: ");
    fgets(reg_data.first_name,sizeof(reg_data.first_name),stdin);
    char *pos = NULL;
    if ((pos=strchr(reg_data.first_name, '\n')) != NULL)
	*pos = '\0';
    printf("\n");
    printf("Enter last name: ");
    fgets(reg_data.last_name,sizeof(reg_data.first_name),stdin);
    pos = NULL;
    if ((pos=strchr(reg_data.last_name, '\n')) != NULL)
	*pos = '\0';
    printf("\n");
    */
/*
 * print the menu
 */
int max_num_orders = 1;
#undef _
#define _(V)					\
    printf("%d : "#V,max_num_orders++);			\
    printf("\n");

    foreach_order_type
    
    int n =0;
    char order[64];
    char order_tok[64];
    char *tok;
    printf("Enter the numbers next to the items you can provide separated by spaces: ");

renter_items:
    memset(order,'\0',sizeof(order));
    memset(reg_data.capability,UNKNOWN,sizeof(reg_data));
    fgets (order, sizeof(order)-1, stdin);
    char *pos = NULL;
    if ((pos=strchr(order, '\n')) != NULL)
	*pos = '\0';
    strcpy(order_tok,order);
    tok = strtok(order_tok," ");
    while(tok!=NULL)
    {
	    int order = atoi(tok);
	    if(order == 0 || order >= max_num_orders)
	    {
		    printf ("Invalid item %s, \nrenter your items:",tok);
		    goto renter_items;
	    }
	    reg_data.capability[(uint8_t) order] = CAPABLE;
	    tok=strtok(NULL," ");
    }

    pkt_t pkt;
    pkt.type = REG_COOK;
    pkt.size = sizeof(reg_data);
    pkt.u.reg_cook_pkt = reg_data;
    write(sockfd, &pkt, sizeof(pkt_t));
wait_for_order:
    memset(&pkt,0,sizeof(pkt_t));
    printf("WELCOME! Please stand by as we find a customer for you\n");
    n = read(sockfd,&pkt,sizeof(pkt_t));
    if(n<=0)
    {
        printf("\n error on read, exiting\n");
	return -1;

    }
#undef _
#define _(V,v)											\
	case V:											\
	v##_handler((v##_pkt_t *) &(pkt.u.v##_pkt), (void *)&sockfd);				\
	break;
	
	switch(pkt.type)
	{
		foreach_cook_pkt_type
	}
    goto wait_for_order;
    return 0;
}
