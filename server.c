#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdint.h>

#include "constants.h"
#include "group_list.h"

uint32_t recv_uint32_from_sock(int connfd)
{
	uint32_t num;
	read(connfd, &num,sizeof(num));
	return ntohl(num);
}

void process_client(int connfd, group_list_t ** group_list)
{
	int32_t group_id = recv_uint32_from_sock(connfd);
	char recv_buff[MAX_SERV_RECV_BUF_SIZE];
	if(*group_list == NULL)
		*group_list = get_new_group_list_item(group_id);
	int n = read(connfd, &recv_buff, sizeof(recv_buff));
	write_message(group_list, group_id, (char *)&recv_buff, sizeof(recv_buff));
	print_group_list(*group_list);

}

int main(int argc, char *argv[])
{
	int32_t listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr; 
	group_list_t * group_list = NULL;
	char send_buff[MAX_SERV_SEND_BUF_SIZE];
	time_t ticks; 

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(send_buff, '0', sizeof(send_buff)); 

	serv_addr.sin_family = AF_INET;
	// accept connections from all IPs of this machine
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERVER_PORT); 

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

	listen(listenfd, MAX_NUM_OF_CLIENTS); 

	printf("SERVER started at port: %d\n",SERVER_PORT);

	while(1)
	{
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
		/*
		   ticks = time(NULL);
		   snprintf(sendBuff, sizeof(sendBuff), "Response from server: %.24s\r\n", ctime(&ticks));
		   write(connfd, sendBuff, strlen(sendBuff)); 
		   */
		process_client(connfd, &group_list);
		close(connfd);
		sleep(1);
	}
}
