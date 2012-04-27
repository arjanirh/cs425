
#include <assert.h>
#include <stdint.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <map>
#include <iostream>
#include <time.h>

using namespace std;

struct node_info{

	int id;
	int port;
};

struct file_info{

	string name;
	string data;
};

/* Function prototypes */
string get_ADD_FILE_result_as_string(const char *fname, const int32_t key, const int32_t nodeId);
string get_DEL_FILE_result_as_string(const char *fname, const int32_t key, const bool deleted, const int32_t nodeId);
string get_GET_FILE_result_as_string(const char *fname,const int32_t key,const bool found,const int32_t nodeId,const char *fdata);
string get_GET_TABLE_result_as_string(const vector<node_info>& finger_table,const uint32_t m,const uint32_t myid,
											const uint32_t idx_of_entry1,const std::map<int32_t, file_info>& keys_table);
string get_finger_table_as_string(const std::vector<node_info>& table,const uint32_t m,const uint32_t myid, const uint32_t idx_of_entry1);
string get_keys_table_as_string(const std::map<int32_t, file_info>& table);
void check_usage(int);
void parse_args(int argc, char **argv);
void make_asserts();
void setup_successor();
void setup_finger_table();
void setup_key_table();
void setup_listener_thread();
void setup_stabilize_thread();
void setup_fixfinger_thread();


int find_successor(int key_to_find, int origin_port);


/*	Globals */	
int opt;
int long_index;

int m = -1;
int id = -1;
int port = -1;
int introducerPort = -1;
int stabilizeInterval = 1;
int fixInterval = 1;
int seed = -1;
const char *logconffile = NULL;
 
// My Successor and my predecessor
struct node_info my_suc = { 0, port};
struct node_info my_pre = {-1, -1};

//finger table: a vector of <(id, port)>
vector<node_info> ftable;

//key table: a map
map<int32_t, file_info> key_table; 

pthread_t stabilize_thread;
pthread_t fixfinger_thread;


/********************************************************/

/* use the following for the main function of your Chord-node file
 * (e.g., analogous to the "MyService_server.cpp" in the thrift
 * example).
 */

#include <getopt.h>
#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    //INIT_LOCAL_LOGGER();
	
	check_usage(argc);
	parse_args(argc, argv);
	make_asserts();

	// configureLogging(logconffile);     /* if you want to use the log4cxx, uncomment this */
	
	//Node Join:
	setup_server();

	setup_successor();
	setup_finger_table();
	setup_key_table();

	setup_stabilize_thread();			//FIX keys also
	setup_fixfinger_thread();

	
	
}

void setup_server(){


}

//ASK suc for keys that belong to me.
// key <=myid
void fixKeys(){

	//Connect to suc.port
	map<int_32, file_info> newmap = rpc_transfer_keys(id);

	//Now add the newly recd keys to my map
	map<int_32, file_info>::iterator it;
	key_table.insert(newmap.start(), newmap.end());
}

/*
 * Ask suc for pred, if diff than me, then set, and notify new succ
 */ 
void *stabilize(){
	
	while(1){
		//Connect to suc.port
		
		struct node_info recd_pre = rpc_return_predecessor();
		
		int inflated_suc_id = my_suc.id;
		if(my_suc.id == 0)
			inflated_suc_id = pow(2,m);
		if(recd_pre.id > id && recd_pre.id < inflated_suc_id){
			my_suc = recd_pre;
		}
	
		//Now notify successor that i am your pred
		//Connect to suc.port --NEED THIS AGAIN because suc might have changed in the if case
		struct node_info my_node_info = {id, port};
		rpc_notify_of_predecessor(my_node_info);

		fixKeys();
		//Sleep for an interval
		sleep(stabilizeInterval);
	}

}

// Make thread 
void setup_stabilize_thread(){

	pthread_create(&stabilize_thread, NULL, stabilize, NULL);	

}

//Fix random finger
void *fix_fingers(){

	while(1){
		int finger = rand() % m;
	
		//Ask myself for finding successor for a particular key
		ftable[finger].id = rpc_find_successor(id);		
		sleep(fixInterval);
	}
}

void setup_fixfinger_thread(){

	pthread_create(&fixfinger_thread, NULL, fix_fingers, NULL);	

}

/*
 * if introducer (id=0) 
 * 	then S=0, P=nil
 * else
 * 	find_successor()
 */
void setup_successor(){

	//If introducer then I am my suc
	if(id==0){
		my_suc.id = 0;
		my_suc.port = port;
	}
	else{
		//Ask id0 to find my suc
		
		//Connect to id0 port
		//Add thrift code here
		my_suc = rpc_find_successor(id);		
	}

}


void setup_finger_table(){

	//if introducer then always copy myself m times into ftable
	if(id==0){
		struct node_info entry= { id, port };

		for(int i=0;i<m; i++){
			ftable.push_back(entry);
		}
	}
	else{
		//TODO:Copy over successor's ftable

		//Thrift: connect to my_suc.port
		ftable = rpc_return_finger_table();
		
	}
}

void setup_key_table(){

	if(id==0){
		//ignore
	}
	else{
		//TODO: copy from successor

		//Thrift: connect to my_suc.port
		key_table = rpc_return_key_table();
	}

}


int find_successor_in_local_ftable(){


	return -1;
}

/* 
 */
int find_successor(int key_to_find, int origin_port){
	
	//If I need my successor, then ask node0


	//else Try to find successor in local ftable
	

	//If not found, check table for closest predecessor
	//	if I am closest pred, then send my successor_port (from finger table) to requestor
	//	else ask closest predecessor for key_to_find
	

	


	return -1;
}

void check_usage(int argc){

	if(argc<4){
		cout<<"Usage: ./node --m --id --port (--introducerPort) (--stabilizeInterval) (--fixInterval) (--seed) (--logConf)\n";
		cout<<"Exiting...\n";
		exit(-1);
	}
}

void make_asserts(){

    assert((m >= 3) && (m <= 10));

    assert((id >= 0) && (id < pow(2,m)));

    assert (port > 0);

	//Seed random number generator
	if(seed == -1){
		srand(time(NULL));
	}
	else{
		srand(seed);
	}
}

void parse_args(int argc, char **argv){

    struct option long_options[] = {
        /* mandatory args */

        {"m", required_argument, 0, 1000},

        /* id of this node: 0 for introducer */
        {"id", required_argument, 0, 1001},

        /* port THIS node will listen on, at least for the
         * Chord-related API/service
         */
        {"port", required_argument, 0, 1002},



        /* optional args */

        /* if not introducer (id != 0), then this is required: port
         * the introducer is listening on.
         */
        {"introducerPort", required_argument, 0, 1003},

        /* path to the log configuration file */
        {"logConf", required_argument, 0, 1004},

        /* intervals (seconds) for runs of the stabilization and
         * fixfinger algorithms */
        {"stabilizeInterval", required_argument, 0, 1005},
        {"fixInterval", required_argument, 0, 1006},

        {"seed", required_argument, 0, 1007},

        {0, 0, 0, 0},
    };


    while ((opt = getopt_long(argc, argv, "", long_options, &long_index)) != -1)
    {

        switch (opt) {
        case 0:
            if (long_options[long_index].flag != 0) {
                break;
            }
            printf("option %s ", long_options[long_index].name);
            if (optarg) {
                printf("with arg %s\n", optarg);
            }
            printf("\n");
            break;

        case 1000:

            m = strtol(optarg, NULL, 10);
            assert((m >= 3) && (m <= 10));
            break;

        case 1001:
            id = strtol(optarg, NULL, 10);
            assert(id >= 0);
            break;

        case 1002:
            port = strtol(optarg, NULL, 10);
            assert(port > 0);
            break;

        case 1003:
            introducerPort = strtol(optarg, NULL, 10);
            assert(introducerPort > 0);
            break;

        case 1004:
            logconffile = optarg;
            break;

        case 1005:
            stabilizeInterval = strtol(optarg, NULL, 10);
            assert(stabilizeInterval > 0);
            break;

        case 1006:
            fixInterval = strtol(optarg, NULL, 10);
            assert(fixInterval > 0);
            break;

        case 1007:
            seed = strtol(optarg, NULL, 10);
            break;

        default:
            exit(1);
        }
    }



}
/*
     * example output:
fname= foo.c
key= 3
added to node= 4

    */      
    string get_ADD_FILE_result_as_string(const char *fname,
                                         const int32_t key,
                                         const int32_t nodeId)
    {
        std::stringstream s;
        s << "fname= " << fname << "\n";
        s << "key= " << key << "\n";
        s << "added to node= " << nodeId << "\n";
        return s.str();
    }

    /*
     * example output:
fname= foo.c
key= 3
file not found

     * example output:
fname= bar.h
key= 6
was stored at node= 0
deleted

    */
    string get_DEL_FILE_result_as_string(const char *fname,
                                         const int32_t key,
                                         const bool deleted,
                                         const int32_t nodeId)
    {
        std::stringstream s;
        s << "fname= " << fname << "\n";
        s << "key= " << key << "\n";
        if (deleted) {
            // then nodeId is meaningful
            s << "was stored at node= " << nodeId << "\n";
            s << "deleted\n";
        }
        else {
            // assume that this means file was not found
            s << "file not found\n";
        }
        return s.str();
    }

    /*
     * example output:
fname= foo.c
key= 3
file not found

     * example output:
fname= bar.h
key= 6
stored at node= 0
fdata= this is file bar.h

     */
    string get_GET_FILE_result_as_string(const char *fname,
                                         const int32_t key,
                                         const bool found,
                                         const int32_t nodeId,
                                         const char *fdata)
    {
        std::stringstream s;
        s << "fname= " << fname << "\n";
        s << "key= " << key << "\n";
        if (found) {
            // then nodeId is meaningful
            s << "stored at node= " << nodeId << "\n";
            s << "fdata= " << fdata << "\n";
        }
        else {
            // assume that this means file was not found
            s << "file not found\n";
        }
        return s.str();
    }

    /* example output (node has 2 files):
finger table:
entry: i= 1, interval=[ 5, 6), node= 0
entry: i= 2, interval=[ 6, 0), node= 0
entry: i= 3, interval=[ 0, 4), node= 0
keys table:
entry: k= 1, fname= 123.doc, fdata= this is file 123.doc data
entry: k= 3, fname= 123.txt, fdata= this is file 123.txt data

     * example output (node has no file):
finger table:
entry: i= 1, interval=[ 1, 2), node= 4
entry: i= 2, interval=[ 2, 4), node= 4
entry: i= 3, interval=[ 4, 0), node= 4
keys table:

    *
    */
    string get_GET_TABLE_result_as_string(
        const vector<node_info>& finger_table,
        const uint32_t m,
        const uint32_t myid,
        const uint32_t idx_of_entry1,
        const std::map<int32_t, file_info>& keys_table)
    {
        return get_finger_table_as_string(
            finger_table, m, myid, idx_of_entry1) + get_keys_table_as_string(keys_table);
    }


/*
 * use this get_finger_table_as_string() function. when asked for its
 * finger table, a node should respond with the string returned by
 * this function.
 */


/* "..." is some struct/class that contains a member "id" as the id of
 * the node pointed to by that entry.
 *
 * myid is the id of the node
 * calling this function.
 */

std::string
get_finger_table_as_string(const std::vector<node_info>& table,
                           const uint32_t m,
                           const uint32_t myid,
                           const uint32_t idx_of_entry1)
{
    std::stringstream s;
    assert(table.size() == (idx_of_entry1 + m));
    s << "finger table:\n";
    for (size_t i = 1; (i - 1 + idx_of_entry1) < table.size(); ++i) {
        using std::setw;
        s << "entry: i= " << setw(2) << i << ", interval=["
          << setw(4) << (myid + (int)pow(2, i-1)) % ((int)pow(2, m))
          << ",   "
          << setw(4) << (myid + (int)pow(2, i)) % ((int)pow(2, m))
          << "),   node= "
          << setw(4) << table.at(i - 1 + idx_of_entry1).id
          << "\n";
    }
    return s.str();
}

/********************************************************/

/*
 * use this get_keys_table_as_string() function. when asked for its
 * keys table, a node should respond with the string returned by this
 * function.
 */


/* "..." is some struct/class that contains members "name" and "data"
 * as the name and data of the file.
 */

std::string
get_keys_table_as_string(const std::map<int32_t, file_info>& table)
{
    std::stringstream s;
    std::map<int32_t, file_info>::const_iterator it = table.begin();
    /* std::map keeps the keys sorted, so our iteration will be in
     * ascending order of the keys
     */
    s << "keys table:\n";
    for (; it != table.end(); ++it) {
        using std::setw;
        /* assuming file names are <= 10 chars long */
        s << "entry: k= " << setw(4) << it->first
          << ",  fname= " << setw(10) << it->second.name
          << ",  fdata= " << it->second.data
          << "\n";
    }
    return s.str();
}

