#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "group_list.h"


// group list functions

// returns group_list_item with the specified group in the given list, NULL if item not present.
group_list_t * get_group_list(group_list_t * const group_list_head, int group)
{
	if(!group_list_head)
		return NULL;
	group_list_t *group_list_item = group_list_head;
	while(group_list_item){
		if(group_list_item->group == group)
			return group_list_item;
		group_list_item = group_list_item->next_item;
	}
	return NULL;
}

group_list_t * get_group_list_end(group_list_t * const group_list_head)
{
	if(!group_list_head)
		return NULL;
	group_list_t *group_list_item = group_list_head;
	while(group_list_item){
		if(!group_list_item->next_item)
			return group_list_item;
		group_list_item = group_list_item->next_item;
	}
	// never hit
	return NULL;
}

//returns the grouplist item for the group, or the insertion point for the group 
//in the list if the group is not found in the list. caller to verify the group id.
group_list_t * get_group_list_item_or_insert_point(group_list_t * const group_list_head, int group)
{
	assert(group_list_head);
	if(group_list_head->group > group)
		return NULL;
	group_list_t *group_list_item = group_list_head;
	while(group_list_item){
		if(group_list_item->group == group)
			return group_list_item;
		// end of list
		if(!group_list_item->next_item)
			return group_list_item;
		//insert point
		if(group_list_item->next_item->group > group)
			return group_list_item;
		group_list_item = group_list_item->next_item;
	}
	// never hit
	return NULL;
}

group_list_t * get_new_group_list_item(int group)
{
	group_list_t * group_list_item = (group_list_t *) malloc(sizeof(group_list_t));
	group_list_item->group = group;
	group_list_item->next_item = NULL;
	group_list_item->msg_list_head = NULL;
	return group_list_item;
}

// create a group list item if group id not present in the list.
// group ids stored in a sorted fashion
group_list_t * get_group_list_for_write(group_list_t ** const group_list_head, int group)
{
	assert(group_list_head);
	group_list_t * group_list_item = get_group_list_item_or_insert_point(*group_list_head, group);
	if(group_list_item && group_list_item->group == group)
		return group_list_item;
	group_list_t * new_item = get_new_group_list_item(group);
	if(!group_list_item){
		new_item->next_item = *group_list_head;
		*group_list_head = new_item;
	}
	else {
		new_item->next_item = group_list_item->next_item;
		group_list_item->next_item = new_item;
	}
	return new_item;
}

// message list functions
msg_list_t * get_msg_list_end(msg_list_t * const msg_list_head)
{
	if(!msg_list_head)
		return NULL;
	msg_list_t *msg_list_item = msg_list_head;
	while(msg_list_item){
		if(!msg_list_item->next_item)
			return msg_list_item;
		msg_list_item = msg_list_item->next_item;
	}
	// never hit
	return NULL;
}

msg_list_t * get_new_msg_list_item()
{
	msg_list_t * msg_list_item = (msg_list_t *) malloc(sizeof(msg_list_t));
	msg_list_item->next_item=NULL;
	msg_list_item->mesg_size=0;
	msg_list_item->mesg=NULL;
	return msg_list_item;
}

void write_message(group_list_t ** group_list_head, int group, char * message, int msg_size)
{
	group_list_t * group_item = get_group_list_for_write(group_list_head, group);
	msg_list_t * msg_item = group_item->msg_list_head;
	if(!msg_item){
		msg_item = get_new_msg_list_item();
		group_item->msg_list_head = msg_item;
	} else {
		msg_list_t * msg_item_temp = get_msg_list_end(msg_item);
		msg_item = get_new_msg_list_item();
		msg_item_temp->next_item = msg_item;

	}
	char * store_mesg = (char *) malloc(msg_size+1);
	memset(store_mesg, '\0', msg_size+1);
	strncpy(store_mesg,message,msg_size-1);
	msg_item->mesg = store_mesg;
	msg_item->mesg_size = msg_size;
	return;
}

void print_msg_list(msg_list_t * const msg_list_head)
{
	msg_list_t * msg_item = msg_list_head;
	int count = 1;
	while(msg_item){
		printf("\t%d: %s\n",count,msg_item->mesg);
		msg_item = msg_item->next_item;
		count++;
	}
}

void print_group_list(group_list_t * const group_list_head)
{
	printf("\n\n\n================================================================\n");
	printf("MESSAGES UPDATE\n");
	printf("================================================================\n");
	group_list_t * group_item = group_list_head;
	while(group_item){
		printf("GROUP: %d\n",group_item->group);
		print_msg_list(group_item->msg_list_head);
		group_item = group_item->next_item;
	}
}
