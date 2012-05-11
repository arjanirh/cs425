/*
 * Node.cpp
 * @Authors: Arjan Singh Nirh (arjanirh@gmail.com)
 * 			 Shpendi Jashari (sjashari112358@gmail.com)
 */			 

#include "MyService.h"
#include <protocol/TBinaryProtocol.h>

#include <server/TSimpleServer.h>

#include <transport/TSocket.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>

#include <server/TThreadedServer.h>

using namespace ::apache::thrift::concurrency;


#include <getopt.h>
#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <io.h>
#endif
#include <fcntl.h>
#include "sha1.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::mp2;

using namespace std;

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
int32_t hash(string fname);
void init_node_info_structs();


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
struct node_info my_suc;
struct node_info my_pre;

//finger table: a vector of <(id, port)>
vector<node_info> ftable;

pthread_mutex_t suc_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pre_mutex = PTHREAD_MUTEX_INITIALIZER;


//key table: a map
map<int32_t, file_info> key_table; 

pthread_t stabilize_thread;
pthread_t fixfinger_thread;

class MyServiceHandler : virtual public MyServiceIf {
 public:
  MyServiceHandler() {
  }

/*
 * Gives current finger table
 */ 
  void rpc_return_finger_table(std::vector<node_info> & _return) {
	_return = ftable;
	}

/*
 * Finds the successor for key
 * First looks at self, returns itself it has same key
 * 		Then goes through finger table backwards and returns key if correct range found
 * 		If not found in ftable then ask closest predecessor for successor of that key
 * 	then return the result to requestor
 *
 * @param key: the key for which to find suc for
 * @return: the <node id, node port> of the successor of key
 */ 
  void rpc_find_successor(node_info& _return, const int32_t key) {

	//Check self and return if equal
	if(id == key){
		_return.id = id;
		_return.port = port;
		return;
	}

	//If I am my own successor (node0 = node0.successor), then return it, dont continue to loop forever
	pthread_mutex_lock(&suc_mutex);
	if(id == my_suc.id){
		_return = my_suc;
		pthread_mutex_unlock(&suc_mutex);
		return;
	}

	//Check if key is between myid and my_suc
	//inflate successor
	int inflated_suc_id = my_suc.id;
	if(my_suc.id ==0){
		inflated_suc_id = pow(2,m);
	}
		
	//Check if key belongs to me
	if(key>id && key<= inflated_suc_id){

		_return = my_suc;
		pthread_mutex_unlock(&suc_mutex);
		return;
	}
	else{
	pthread_mutex_unlock(&suc_mutex);
	//ask closest predecessor - go through finger table and send request to that predecessor
		struct node_info target_pre;
  		find_predecessor(target_pre, key);

		//Now connect to predecessor and ask for its successor
			node_info result;
		if(id != target_pre.id){			
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", target_pre.port));
			boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
			boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
			
			MyServiceClient client(protocol);
			transport->open();
			client.rpc_find_successor(result, key);
			transport->close();
		}
		else{
			//rpc_find_successor(result, key);
		}
		
		_return = result;
		return;
	}
	
}

//Node asks for keys that belong to him
/* @param: key: requesting node's id so only required keys are sent back
 */ 
void rpc_transfer_keys(std::map<int32_t, file_info> & _return, const int32_t key) {
	
	//Go through key table 
	std::map<int32_t, file_info>::iterator it;
	for(it = key_table.begin(); it!= key_table.end(); it++){
		if(it->first <= key){
				//If found, then insert into returning map and remove from my keytable
			_return.insert(*it);
			key_table.erase(it);	
		}
	}
	
}

/*
 * When a node asks for my local successor, and not to call find successor on me
 */ 
  void rpc_give_local_successor(node_info& _return) {
	pthread_mutex_lock(&suc_mutex);
	_return = my_suc;
	pthread_mutex_unlock(&suc_mutex);
  }

/* Return the predecessor for a particular key
 * @return: the node id and port struct of predecessor
 */ 
  void find_predecessor(node_info& _return, const int32_t key) {

	int inflated_key = key;
	if(key<id){		//Tke care of wrap arounds
		inflated_key = key + pow(2,m);
	}
	
	//Go through finger table and find predecessor
	for(int i=m-1;i>=0;i--){

		int inflated_ftable_id = ftable[i].id;
		if(ftable[i].id < id){
			inflated_ftable_id = ftable[i].id + pow(2,m);
		}

		if((inflated_key >= inflated_ftable_id) && (inflated_ftable_id > id)){
			_return = ftable[i];
			return;
		}
	}
	_return.id = id;
	_return.port = port;
	return;
  }

/*
 * Add a file to some node.
 * @param: key: if -1 then hash key and pass it along, else
 * find successor for key, and send file to that node
 * @return: the information about which node added the file
 */ 
  void rpc_add_file(std::string& _return, const std::string& filename, const std::string& data, const int32_t key) {
	
	//If listener make call, then first hash filename and then search for right place to insert
	int target_key = key;
	if(key == -1){
		target_key = hash(filename);
	}
	int inflated_id = id;
	if(id ==0){
		inflated_id = pow(2,m);
	}
	
	pthread_mutex_lock(&pre_mutex);
	int preid = my_pre.id;
	pthread_mutex_unlock(&pre_mutex);
	//Check if this key belongs to me, if it does,  then insert into my table
	if((target_key >preid && target_key <=inflated_id )|| (preid == id )){		
		struct file_info filedata;
		filedata.name = filename;
		filedata.data = data;
		key_table.insert(pair<int32_t, file_info>(target_key, filedata));
		cout<<"node= "<<id<<": added file: k= "<<target_key<<endl; 
		_return = get_ADD_FILE_result_as_string(&filename[0], target_key, id);
		return;
	}
	//Not my file, then find successor for this key and send file to that node
	else{
		struct node_info target_suc;
		rpc_find_successor(target_suc, target_key);

		//Connect to target_suc.port
			string result;
			//Have sanity check so you dont send yourself the rpc command
		if(target_suc.id != id){
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", target_suc.port));
			boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
			boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
			
			MyServiceClient client(protocol);
			transport->open();
			client.rpc_add_file(result, filename, data, target_key);
			transport->close();
		}
		else{
			rpc_add_file(result, filename, data, target_key);
		}
		_return = result;
		return;
	}
  }
/*
 * Similar to add_file, but deletes it instead
 */ 
  void rpc_del_file(std::string& _return, const std::string& filename, const int32_t key) {

	//If listener make call, then first hash filename and then search for right place to insert
	int nkey = key;
	if(key == -1){
		nkey = hash(filename);
	}

	//If file should be in my key table
	int inflated_id = id;
	if(id ==0){
		inflated_id = pow(2,m);
	}

	pthread_mutex_lock(&pre_mutex);
	int preid = my_pre.id;
	pthread_mutex_unlock(&pre_mutex);
	if((nkey >preid && nkey <=inflated_id) || (preid == id)){
		//If key not found
		if(key_table.find(nkey) == key_table.end()){
			cout<<"node= "<<id<<": no such file k= "<<nkey<<" to delete"<<endl;
			_return = get_DEL_FILE_result_as_string(&filename[0], nkey, false, -1);
			return;
		}
		//if file found then delete from map
		key_table.erase( key_table.find(nkey) ); 
		cout<<"node= "<<id<<": deleted file: k= "<<nkey<<endl; 
		_return = get_DEL_FILE_result_as_string(&filename[0], nkey, true, id);
		return;
	}

	//Not my file
	else{
		//local call, no need to connect
		struct node_info target_suc;
		rpc_find_successor(target_suc, nkey);
		//Connect to target_suc.port
		if(target_suc.id != id){

			boost::shared_ptr<TSocket> socket(new TSocket("localhost", target_suc.port));
			boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
			boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
			
			MyServiceClient client(protocol);
			transport->open();
			client.rpc_del_file(_return, filename, nkey);
			transport->close();
		}
		else{
			rpc_del_file(_return, filename, nkey);
		}
		return;
	}

  }
/*
 * Search for file, and retrieve information about where it is stored
 * @param: key: -1 if listener calls this rpc, in which case need to hash it
 */ 
  void rpc_get_file(std::string& _return, const std::string& filename, const int32_t key) {

	//If listener make call, then first hash filename and then search for right place to insert
	int nkey = key;
	if(key == -1){
		nkey = hash(filename);
	}

	pthread_mutex_lock(&pre_mutex);
	int preid = my_pre.id;
	pthread_mutex_unlock(&pre_mutex);
	//If file should be in my key table
	if((nkey >preid && nkey <=id) || (preid == id)){
		//If key not found
		if(key_table.find(nkey) == key_table.end()){
			cout<<"node= "<<id<<": no such file k= "<<nkey<<" to serve"<<endl;
			_return = get_GET_FILE_result_as_string(&filename[0], nkey, false, -1, NULL);
			return;
		}
		//if file found then return fdata from map
		struct file_info finfo = (key_table.find(nkey))->second; 
		cout<<"node= "<<id<<": served file: k= "<<nkey<<endl; 
		_return = get_GET_FILE_result_as_string(&(finfo.name)[0], nkey, true, id, &(finfo.data)[0]);
		return;
	}

	//Not my file
	else{
		//local call, no need to connect
		struct node_info target_suc;
		rpc_find_successor(target_suc, nkey);
		//Connect to target_suc.port
		if(target_suc.id != id){
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", target_suc.port));
			boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
			boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
			
			MyServiceClient client(protocol);
			transport->open();
			client.rpc_get_file(_return, filename, nkey); 
			transport->close();
		}
		else{
			rpc_get_file(_return, filename, nkey); 
		}
		return;
	}

  }

/*
 * Get ftable and key table for a node
 * @param: key: the node for which info is needed
 */
  void rpc_get_table(std::string& _return, const int32_t key) {

	if(key == id){
		_return = get_GET_TABLE_result_as_string(ftable, m, key, 0, key_table);
		return;
	}
	else{
	struct node_info target_suc;
	rpc_find_successor(target_suc, key);
	
	
	//Connect to target_suc
		if(target_suc.id != id){
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", target_suc.port));
			boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
			boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
			
			MyServiceClient client(protocol);
			transport->open();
			client.rpc_get_table(_return, key); 
			transport->close();
		}
		else{
			rpc_get_table(_return, key); 
		}
		
	return;
	}
 
  }

/*
 * When some node asks for my key table, then return it
 * Called when new node joins. it needs some keys
 */ 
  void rpc_return_key_table(std::map<int32_t, file_info> & _return) {
  	_return = key_table;
  }

/*
 * Returns my predecessor
 */ 
  void rpc_return_predecessor(node_info& _return) {
	pthread_mutex_lock(&pre_mutex);
  	_return = my_pre;
	pthread_mutex_unlock(&pre_mutex);
  }

/*
 *When some node realizes current node is its successor, then it notifies it so this node can set its predecessor right
 @return: nothing
 @param: the new predecessor that i need to set
 */
  void rpc_notify_of_predecessor(const node_info& new_pre) {

	//cout<<"[node"<<id<<"] notified of pred.id="<<new_pre.id<<endl;
	int inflated_id = id;
	if(id == 0){
		inflated_id = pow(2,m);
	}
	pthread_mutex_lock(&pre_mutex);
	if(my_pre.id == -1 || (new_pre.id > my_pre.id && new_pre.id < inflated_id) || (my_pre.id == id) ){
		my_pre = new_pre;
		//cout<<"node= "<<id<<": updated predecessor= "<<my_suc.id<<endl;
		if(my_pre.id != new_pre.id){
			cout<<"node= "<<id<<": updated predecessor= "<<my_pre.id<<endl; 
		}
	}
	pthread_mutex_unlock(&pre_mutex);
  }

};

int main(int argc, char **argv) {

	
	init_node_info_structs();
	check_usage(argc);
	parse_args(argc, argv);
	make_asserts();

	//Node Join:
	setup_successor();
	setup_finger_table();
	setup_key_table();

	//Start threads
	setup_stabilize_thread();			//FIX keys also
	setup_fixfinger_thread();

	//Set up server
  shared_ptr<MyServiceHandler> handler(new MyServiceHandler());
  shared_ptr<TProcessor> processor(new MyServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TThreadedServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();

	while(1);			//run forever (until listener ends)
	
  return 0;
}

//Because structs cannot be initialized with {..}
void init_node_info_structs(){

	my_suc.id = 0;
	my_suc.port = port;
	my_pre.id = -1;
	my_pre.port = -1;

}

/*
 * Wrapper for the given SHA1 hash funtion
 * @param: the filename to be hashed
 * @return the key it hashes to
 */ 
int32_t hash(string fname){

	SHA1Context sha;
	SHA1Reset(&sha);
	unsigned int strsize = strlen(&fname[0]);
	SHA1Input(&sha,(unsigned char*) &fname[0], strsize);		//todo: check if strlen works 

	if(!SHA1Result(&sha)){
		cerr<<"key_gen_test: could not compute key ID for "<<fname<<endl;
		return -1;
	}
	int32_t new_key_id = sha.Message_Digest[4]%((int)pow(2,m));
	return new_key_id;
}

//ASK suc for keys that belong to me.
void fixKeys(){

	map<int32_t, file_info> newmap;
	//Connect to suc.port

	pthread_mutex_lock(&suc_mutex);
	if(my_suc.id != id){
		boost::shared_ptr<TSocket> socket(new TSocket("localhost", my_suc.port));
		pthread_mutex_unlock(&suc_mutex);
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	
		MyServiceClient client(protocol);
   		 transport->open();
		client.rpc_transfer_keys(newmap, id);
		transport->close();
	}
	else{
		pthread_mutex_unlock(&suc_mutex);
	}
	//Now add the newly recd keys to my map
	map<int32_t, file_info>::iterator it;
	key_table.insert(newmap.begin(), newmap.end());
	
}

/*
 * Ask suc for pred, if diff than me, then set, and notify new succ
 */ 
void *stabilize(void* thread_arg){
	
	while(1){
		//Connect to suc.port
		
		struct node_info recd_pre;
		if(my_suc.id !=id){
			pthread_mutex_lock(&suc_mutex);
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", my_suc.port));
			pthread_mutex_unlock(&suc_mutex);
			boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
			boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
		
			MyServiceClient client(protocol);
   			transport->open();
			client.rpc_return_predecessor(recd_pre);
			transport->close();
		}
		else{
				recd_pre = my_pre;
		}
			pthread_mutex_lock(&suc_mutex);
			int inflated_suc_id = my_suc.id;
			if(my_suc.id == 0)
				inflated_suc_id = pow(2,m);
			if(recd_pre.id > id && recd_pre.id < inflated_suc_id){
				my_suc = recd_pre;
			}
		
			//Now notify successor that i am your pred
			//Connect to suc.port --NEED THIS AGAIN because suc might have changed in the if case
			boost::shared_ptr<TSocket> n_socket(new TSocket("localhost", my_suc.port));
			pthread_mutex_unlock(&suc_mutex);
			boost::shared_ptr<TTransport> n_transport(new TBufferedTransport(n_socket));
			boost::shared_ptr<TProtocol> n_protocol(new TBinaryProtocol(n_transport));
		
			MyServiceClient n_client(n_protocol);
		    n_transport->open();
			struct node_info my_node_info;
			my_node_info.id = id;
			my_node_info.port = port;
	
			n_client.rpc_notify_of_predecessor(my_node_info);
			n_transport->close();
		
			fixKeys();
			//Sleep for an interval
			sleep(stabilizeInterval);
	
		}
	return NULL;
}

// Make thread 
void setup_stabilize_thread(){
	
	int ret = 1;
	while(ret != 0){
		ret = pthread_create(&stabilize_thread, NULL, stabilize, NULL);	
	}
}

//Fix random finger
void *fix_fingers(void* thread_arg){

	while(1){
		if(my_suc.id != id){
			int finger = rand() % m;
	
			//Ask myself for finding successor for a particular key
			MyServiceHandler client;
			struct node_info result;
			int target_key = (int)(id + pow(2,finger)) % (int)(pow(2,m));
			client.rpc_find_successor(result, target_key);		
			if(ftable[finger].id !=result.id){
				cout<<"node= "<<id<<": updated finger entry: i= "<<(finger+1)<<", pointer= "<<result.id<<endl; 
			}
			ftable[finger] = result;
		}
		sleep(fixInterval);
	}
	return NULL;
}

void setup_fixfinger_thread(){

	int ret = 1;
	while(ret != 0){
		ret = pthread_create(&fixfinger_thread, NULL, fix_fingers, NULL);	
	}
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
	pthread_mutex_lock(&suc_mutex);
		my_suc.id = 0;
		my_suc.port = port;
	pthread_mutex_unlock(&suc_mutex);
	cout<<"node= "<<id<<": initial successor= "<<my_suc.id<<endl;

	}
	else{
		//Ask id0 to find my suc
		boost::shared_ptr<TSocket> socket(new TSocket("localhost", introducerPort));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	
		MyServiceClient client(protocol);
	    transport->open();
		struct node_info ret_info;
		client.rpc_find_successor(ret_info, id);

	pthread_mutex_lock(&suc_mutex);
		my_suc = ret_info;
	pthread_mutex_unlock(&suc_mutex);
		transport->close();
	cout<<"node= "<<id<<": initial successor= "<<my_suc.id<<endl;
	}

}

/*
 * If node 0 then push myself onto finger table
 * Else, send request to successor and copy its ftable
 */ 
void setup_finger_table(){

	//if introducer then always copy myself m times into ftable
	struct node_info entry;
	if(id==0){
		entry.id = id;
		entry.port = port;
	}
	else{
		entry.id = id;
		entry.port = introducerPort;
	}

	for(int i=0;i<m; i++){
			ftable.push_back(entry);
		}
/*	else{
	pthread_mutex_lock(&suc_mutex);
		int target_port = my_suc.port;
	pthread_mutex_unlock(&suc_mutex);
		boost::shared_ptr<TSocket> socket(new TSocket("localhost", target_port));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	
		MyServiceClient client(protocol);
	    transport->open();
		client.rpc_return_finger_table(ftable);
		transport->close();
		
	}
*/
}

/*
 * If node 0 then ignore (no keys added)
 * Else, send request to successor and copy its key
 */ 
void setup_key_table(){

	if(id==0){
		//ignore
		return;
	}
	else{

	pthread_mutex_lock(&suc_mutex);
	int target_port = my_suc.port;
	pthread_mutex_unlock(&suc_mutex);
		boost::shared_ptr<TSocket> socket(new TSocket("localhost", target_port));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	
		MyServiceClient client(protocol);
	    transport->open();
  		client.rpc_transfer_keys(key_table, id);
		transport->close();
	}

}


/* checks usage
 * */
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

