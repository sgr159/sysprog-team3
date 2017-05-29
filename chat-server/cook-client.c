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
    strcpy(reg_data.first_name,"abc");
    strcpy(reg_data.last_name,"xyz");
    pkt_t pkt;
    pkt.type = REG_COOK;
    pkt.size = sizeof(reg_data);
    pkt.u.reg_cook_pkt = reg_data;
    write(sockfd, &pkt, sizeof(pkt_t));
wait_for_order:
    memset(&pkt,0,sizeof(pkt_t));
    printf("WELCOME! Please stand by as we find a customer for you\n");
    read(sockfd,&pkt,sizeof(pkt_t));
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
