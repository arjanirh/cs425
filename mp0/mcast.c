#include <string.h>
#include <assert.h>

#include "mp0.h"

#define TIMER_INTERVAL 10000

//Function declarations
void *heartbeater(void);
void timer_handler(void);
void setup_timer(void);

//Global variables
int *old_seq;
int *new_seq;
int seq_size;
int sequence_num;
pthread_t heartbeat_thread;
pthread_mutex_t suspend_mutex;
pthread_cond_t suspend_cond;
struct sigaction sa;
struct itimerval timer;



void multicast_init(void) {
    unicast_init();
	
	seq_size = 0;
	sequence_num = 0;
	new_seq = NULL;

	//Make thread that sends out heartbeats and also periodically checks old and new seq arrays
	pthread_create(&heartbeat_thread, NULL, heartbeater, NULL);

	//Create timer that periodically wakes up heartbeater thread
	setup_timer();

}

void setup_timer(){

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &timer_handler;
	sigaction(SIGALRM, &sa, NULL);

	//Configure timer to expire every T time units
	timer.it_value.tv_sec = 0;						//First timeout
	timer.it_value.tv_usec = TIMER_INTERVAL;
	timer.it_interval.tv_sec = 0;					//interval 
	timer.it_interval.tv_usec = TIMER_INTERVAL;

	//start the timer
	setitimer(ITIMER_REAL, &timer, NULL);

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

		//pause or suspend until woken up by timer
		pthread_mutex_lock(&suspend_mutex);
		pthread_cond_wait(&suspend_cond, &suspend_mutex);
		pthread_mutex_unlock(&suspend_mutex);
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
