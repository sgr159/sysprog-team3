#ifndef GROUP_LIST_H
#define GROUP_LIST_H

typedef struct msg_list_t
{
	struct msg_list_t *next_item;
	char *mesg;
	int mesg_size;
} msg_list_t;

typedef struct group_list_t
{
	struct group_list_t *next_item;
	struct msg_list_t *msg_list_head;
	int group;
} group_list_t;

group_list_t * get_new_group_list_item(int group);

void write_message(group_list_t ** const group_list_head, int group, char * message, int msg_size);

void print_group_list(group_list_t * const group_list_head);

#endif      // GROUP_LIST_H
