#include "linkedlist.h"

int linked_list_init(linked_list_t *ll) 
{
	if(ll == NULL){
		printf("linked list pointer error \n");
		return -1;
	}

	/* do program */
	/* allocate linked list */
	ll = (linked_list_t*)malloc(sizeof(linked_list_t));
	/* initialize list head */
	ll->list_head = (node_t*)malloc(sizeof(node_t));
	ll->list_head->key = 0;
	ll->list_head->next = NULL;
	/* initialize mutex lock for BLINKED_LIST version */
	#ifdef BLINKED_LIST
	pthread_mutex_init(&ll->list_lock, NULL);
	ll = (linked_list_t*)malloc(sizeof(linked_list_t));
	/* initialize list head */
	ll->list_head = (node_t*)malloc(sizeof(node_t));
	ll->list_head->key = 0;
	ll->list_head->next = NULL;
	#endif


	srand((unsigned) time (NULL));
	return 0;
}


void linked_list_destroy(linked_list_t* ll)
{
	/* do program */ 
	/* free all nodes in the linked list */
	node_t *tmpnode;
	while(ll->list_head != NULL){
		tmpnode = ll->list_head->next;
		free(ll->list_head);
		ll->list_head = tmpnode;
	}
	/* free the linked list */ 
	free(ll);

	return;
}


long linked_list_get (long key, linked_list_t* ll) 
{
	/* do program */
	node_t *tmpnode = NULL;
	node_t *start = ll->list_head;
	while(start != NULL){
		if(start->key == key){
			return start->value;
		}
	}
	/* if key is found, return value */
	/* if key is not found, return -1 */
	return -1;
}

long linked_list_put (long key, long value, linked_list_t* ll)
{
	
	/* do program */
	/* if succeeds, return 0 */
	/* if fails, return -1 */

	int i = 0;
	node_t *new_node = (node_t*)malloc(sizeof(node_t));
	while (i < 1000){
#ifdef BLINKED_LIST
		// lock 
		pthread_mutex_lock(&ll->list_lock);
		new_node->key = key;
		new_node->value = value;
		new_node->next = ll->list_head->next;
		ll->list_head->next = new_node;
		i++;
		// unlock
		pthread_mutex_unlock(&ll->list_lock);
#else 
		new_node->key = key;
		new_node->value = value;
		new_node->next = ll->list_head->next;
		ll->list_head->next = new_node;
		CAS(&(ll->list_head), ll->list_head->next, &new_node);
		i++;
#endif
	}
	return 0;
}
