/*
 *TODO: 1) should we protect mcast_members and mcast_num_members while reading also?
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mp1.h"


typedef struct _node_{
	int *timestamp;
	char *message;
	int source;

	struct _node_ *prev;
	struct _node_ *next;
}node;



int *my_timestamp;
int vector_len = 0;
int sorted = 0;
int *map;
node *head;
node *tail;

int compare(const void *a, const void *b);
int getindex(int pid);
void add_node(char* original_message,int* incoming_timestamp,int source);
char* concatenate_timestamp(const char* message);
void pop_and_deliver(node ** curr_dbl_ptr);
void check_buffered_messages(int current_process_index, int* is_buffer_ptr, int* is_reject_ptr, int* incoming_vector);
void sort_array();
void shout_state();


void shout_state(){


	debugprintf("vector_len = %d\n", vector_len);


}

void multicast_init(void) {
    unicast_init();
	my_timestamp = malloc(sizeof(int));
	my_timestamp[0] = 0;
	//vector_len = 1;
	head = NULL;
	tail = NULL;
}

/* Basic multicast implementation */
/*
 * 1. increment vector timestamp (increment current process's val in vector)
 * 2. append the vector timestamp to each message
 */ 
void multicast(const char *message) {


	//Check if this is first call to mcast
	if(sorted ==0){
		sort_array();					//sort our local pid array
		sorted = 1;
	}
	
    // increment vector timestamp (increment current process's val in vector)
 	int index = getindex(my_id);

	if(index >=vector_len){
		debugprintf("index=%d is greater than vector len=%d\n", index, vector_len);
		shout_state();
	}
	my_timestamp[index]++;

	/* Append timestamps to beginning of message
	  "hello" --> "1 0 0 hello"
	 */
	char *new_message = concatenate_timestamp(message);

	/* Send ucast with timestamp+message to every process*/
    int i;
	//debugprintf("-----locking mutex and starting for loop on all members : ----\n");
    pthread_mutex_lock(&member_lock);
	//debugprintf("acquired lock\n");
    for (i = 0; i < mcast_num_members; i++) {
		int new_len = strlen(new_message)+1;
		//debugprintf("-----Sending message----\n");
        usend(mcast_members[i], new_message, new_len);
		//usend(mcast_members[i], message, strlen(message)+1);
    }
    pthread_mutex_unlock(&member_lock);
	//debugprintf("-----finished looping over and usending to all Processes: ----\n");
}

/*
 * 1. First parse incoming vector
 * 2. Check incoming vector against current vector: It should be identical AND vector[source] shd be = incomingVec[source]+1
 * 3. if false, then buffer queue message and source and timestamp
 * 4. If true, then deliver and update vector
 * 5. If deliver, then check vectors in queue like step 3 and repeat
 */ 
void receive(int source, const char *message, int len) {
    assert(message[len-1] == 0);

	debugprintf("-----received message----\n");
	int i=0;
	//Check if this is first call to mcast
	if(sorted ==0){
		//sort our local pid array
		sort_array();
		sorted = 1;
	}
	
	//1. Parse into vector and message
	int incoming_timestamp[vector_len];
	for(i=0;i<vector_len; i++){
		sscanf(message+(2*i), "%d ", &(incoming_timestamp[i]));
		debugprintf("timestamp parsed (index %d)=%d\n", i, incoming_timestamp[i]);
	}
	char* original_message = message+(i*2);
	debugprintf("original message parsed = %s\n", original_message);



	//2. check timestamps for ordering
	int is_buffer = 0;
	int is_reject = 0;
	int current_process_index = getindex(source);

	check_buffered_messages(current_process_index, &is_buffer, &is_reject, incoming_timestamp);
	//3. If out of order, then store message and timestamp and the source into queue
	if(is_buffer==1){
		add_node(original_message, incoming_timestamp, source);
	}
	else if(is_reject==1){
		return;
	}
	else{
    	deliver(source, original_message);

		//Copy over timestamp
		for(i=0;i<vector_len;i++)
			my_timestamp[i] = incoming_timestamp[i];
	
		//Now check the queue, if you can deliver any buffered messages
		node* curr = head;
		while(curr!=NULL){

			int index = getindex(curr->source);
			int is_buffer=0, is_reject=0;
			debugprintf("curr->timestamp[0] = %d\n", curr->timestamp[0]);
			check_buffered_messages(index, &is_buffer, &is_reject, curr->timestamp);
			if(is_buffer !=1 && is_reject !=1){
				pop_and_deliver(&curr);
			}
			else		
				curr = curr->next;
		}
	}
}


void pop_and_deliver(node ** curr_dbl_ptr){

	node* curr = *curr_dbl_ptr;
	node* first = curr->prev;
	node* second = curr->next;
	
	if(first!=NULL){
		first->next = second;
	}
	else
		head = curr->next;

	if(second!=NULL)
		second->prev = first;
	else
		tail = curr->prev;

	
	//deliver curr's message
    deliver(curr->source, curr->message);

	node* deleteptr = curr;
	curr = second;
	//deallocate curr
	
	free(deleteptr->message);
	free(deleteptr->timestamp);
	free(deleteptr);

}

void check_buffered_messages(int current_process_index, int* is_buffer_ptr, int* is_reject_ptr, int* incoming_vector){

	int i=0;
	for(i=0;i<vector_len; i++){
		if(i!=current_process_index){
				//should be same
				debugprintf("--- i=%d\n", i);
				if(my_timestamp[i] != incoming_vector[i]){
					*is_buffer_ptr = 1;
					break;
				}
		}
		else{
				if(incoming_vector[i] < my_timestamp[i]){
					*is_reject_ptr = 1;
					break;
				}
				if(incoming_vector[i] - my_timestamp[i] > 1){
					*is_buffer_ptr = 1;
					break;
				}
		}
	}
}
/* Buffer message + timestamp to linked list
*/
void add_node(char* original_message,int* incoming_timestamp,int source){


	node* curr = NULL;
	if(head == NULL){
		curr = malloc(sizeof(node));
		head = curr;
		head->next = NULL;
		head->prev = NULL;
		tail = head;

	}
	else{
		tail->next = malloc(sizeof(node));
		curr = tail->next;
		curr->next = NULL;
		curr->prev = tail;
	}

		curr->source = source;
		int len = strlen(original_message);
		curr->message = malloc(sizeof(char)*(len+1));
		strcpy(curr->message, original_message);
		curr->timestamp = malloc(vector_len * sizeof(int));

		int i=0;
		for(i=0;i<vector_len; i++){
			curr->timestamp[i] = incoming_timestamp[i];
		}

		tail = curr;
}

/* Concatenate timestamp to beginning of message
*	Allocate new memory and append string
*	The caller is responsible for deallocating the memory
*/
char* concatenate_timestamp(const char* message){
	
	//Find the size to be allocated
	int len = vector_len *2 + strlen(message) +1;

	char* new_message = malloc(sizeof(char) * len);
	memset(new_message, 0, len);

	//Concatenate first timestamp (null terminates the new message)
	char temp[3];
	sprintf(temp, "%d ", my_timestamp[0]);
	temp[2] = '\0';
	strcpy(new_message, temp);

	//concatenate each timestamp index
	int i=0;
	for(i=1;i<vector_len; i++){
		char temp[3];
		sprintf(temp, "%d ", my_timestamp[i]);
		temp[2] = '\0';
		strcat(new_message, temp);		
	}

	//concatenate the real message
	strcat(new_message, message);		

	return new_message;
}

int getindex(int pid){

	int i=0;
    //pthread_mutex_lock(&member_lock);
	for(i=0;i<mcast_num_members; i++){
		if(map[i] == pid)
			return i;
	}
    //pthread_mutex_unlock(&member_lock);

	//No pid found
	return 0;

}


void mcast_join(int member) {
	
		vector_len++;
		debugprintf("Called mcast join, vector_len=%d\n", vector_len);
		my_timestamp = realloc(my_timestamp, sizeof(int)* vector_len);
		int i=0;
		for(i=0;i<vector_len; i++){
			my_timestamp[i] = 0;
		}


}

void sort_array(){
	int i=0;

    //pthread_mutex_lock(&member_lock);
	map = malloc(sizeof(int)*mcast_num_members);

	for(i=0;i<mcast_num_members;i++){
		map[i] = mcast_members[i];
	}
	
	qsort(map, mcast_num_members, sizeof(int), compare);
    //pthread_mutex_unlock(&member_lock);

}

int compare(const void *a, const void *b){

	if(a==NULL || b== NULL)
		return 0;
	return ( *((int*)a) - *((int*)b));

}







