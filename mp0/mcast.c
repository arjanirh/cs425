#include <string.h>
#include <assert.h>

#include "mp0.h"

//Function declarations
void *heartbeater(void);


//Global variables
int *old_seq;
int *new_seq;
int seq_size;
int sequence_num;
pthread_t heartbeat_thread;


void multicast_init(void) {
    unicast_init();
	
	seq_size = 0;
	sequence_num = 0;

	//Make thread that sends out heartbeats and also periodically checks old and new seq arrays
	pthread_create(&heartbeat_thread, NULL, heartbeater, NULL);

	new_seq = NULL;
}

void *heartbeater(void){

	while(1){
		mutex_lock(&member_lock);

		//Send out heartbeats to each process in the group
		for(int i=0;i<mcast_num_members; i++){
			if(mcast_members[i] == my_id)
				continue;

			sequence_num++;											//TODO: need to check for overflow? make long? reset?
			char message[256];
			sprintf(message, "%d", sequence_num);
			usend(mcast_members[i], message, sizeof(message));
			
		}
		mutex_unlock(&member_lock);

		//check old and new arrays to find failures
		for(int i=0; i<seq_size; i++){
			//check failure
			if(new_seq[i] <= old_seq[i] && new_seq[i]!=-1){			//-1 means we already detected it before
				printf("[%d]: Process %d has failed.\n", my_id, mcast_members[i]); 
			}
			
			//copy from new to old
			old_seq[i] = new_seq[i];
		}

		//pause or suspend
		sleep(10);			//sleep for 10 msec
	}

}



void receive(int source, const char *message, int len) {
    assert(message[len-1] == 0);
	
	mutex_lock(&member_lock);
	
	//Resize seq arrays if receive from new member
	if(seq_size != mcast_num_members){

		old_seq = realloc(old_seq, sizeof(int)* mcast_num_members);
		new_seq = realloc(new_seq, sizeof(int)* mcast_num_members);

		for(int i=seq_size; i<mcast_num_members; i++){
			old_seq[i] = -1;
			new_seq[i] = 0;
		}
		seq_size = mcast_num_members;
	}

	int index = 0;
	for(int i=0;i<mcast_num_members; i++){
		if(source == mcast_members[i]){
			index = i;
			break;
		}
	}
	
	new_seq[index] = atoi(message);

	mutex_unlock(&member_lock);
}
