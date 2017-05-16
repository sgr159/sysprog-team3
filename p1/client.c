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

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[MAX_CLNT_RECV_BUF_SIZE];
    char sendBuff[MAX_CLNT_SEND_BUF_SIZE];
    struct sockaddr_in serv_addr; 

    if(argc != 4)
    {
        printf("\n Usage: %s <ip of server> <group-id> <message>\n",argv[0]);
        return 1;
    } 

    memset(recvBuff, '0',sizeof(recvBuff));
    memset(sendBuff, '0',sizeof(sendBuff));
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

    int32_t group_id = htonl(atoi(argv[2]));

    write(sockfd, &group_id, sizeof(group_id));
    strcpy(sendBuff,argv[3]);
    sendBuff[sizeof(sendBuff)-1] = '\0';
    write(sockfd, sendBuff, sizeof(sendBuff));

/*
    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    } 

    if(n < 0)
    {
        printf("\n Read error \n");
    } 
*/
    return 0;
}
