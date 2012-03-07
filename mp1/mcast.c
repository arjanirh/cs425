/*
 *TODO: 1) should we protect mcast_members and mcast_num_members while reading also?
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mp1.h"


int *my_timestamp;
int vector_len;
int sorted = 0;
int *map;
int compare(const void *a, const void *b);
int getindex(int pid);

void multicast_init(void) {
    unicast_init();
	my_timestamp = malloc(sizeof(int));
	my_timestamp[0] = 0;
	vector_len = 0;

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
	my_timestamp[i]++;

	/* Append timestamps to beginning of message*/
		


	/* Send ucast with timestamp+message to every process*/
    int i;
    pthread_mutex_lock(&member_lock);
    for (i = 0; i < mcast_num_members; i++) {
        usend(mcast_members[i], message, strlen(message)+1);
    }
    pthread_mutex_unlock(&member_lock);
}

/*
 * 1. First parse incoming vector
 * 2. Check incoming vector against current vector: It should be identical AND vector[source] shd be = incomingVec[source]+1
 * 3. if false, then buffer queue message and source
 * 4. If true, then deliver and update vector
 * 5. If deliver, then check vectors in queue like step 3 and repeat
 */ 
void receive(int source, const char *message, int len) {
    assert(message[len-1] == 0);

	//Check if this is first call to mcast
	if(sorted ==0){
		//sort our local pid array
		sort_array();
		sorted = 1;
	}

    deliver(source, message);
}

int getindex(int pid){

	for(int i=0;i<mcast_num_members; i++){
		if(map[i] == pid)
			return i;
	}

	//No pid found
	return 0;

}


void mcast_join(int member) {
	

		vector_len++;
		my_timestamp = realloc(my_timestamp, sizeof(int)* vector_len);
		int i=0;
		for(i=0;i<vector_len; i++){
			my_timestamp[i] = 0;
		}


}

void sort_array(){
	int i=0;

	map = malloc(sizeof(int)*mcast_num_members);

	for(i=0;i<mcast_num_members;i++){
		map[i] = mcast_members[i];
	}
	
	qsort(map, mcast_num_members, sizeof(int), compare);

}

int compare(const void *a, const void *b){

	if(a==NULL || b== NULL)
		return 0;
	return ( *((int*)a) - *((int*)b));

}







