#include <iostream>
#include <stdio.h>
#include <string>
#include <getopt.h>
#include <stdlib.h>
#include <assert.h>
#include <sstream>
#include <time.h>
#include <stdlib.h>

#include "MyService.h"
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::mp2;

using namespace std;

//Holds info about successor and predecessor
struct node_info{
	int id;
	int port;
};

//Function declarations
void parse_args(int argc, char **argv);
void setup_and_connect_to_node();
void handle_command();
void add_file(string, string);
void del_file(string );
void get_file(string);
void get_table(int);
void add_node(int id);
void donothing(const char*);


//Global variables
int m=-1;
int startingPort = -1;
int currentPort = -1;
int attachToNode = -1;
int stabilizeInterval = 1;		//Default value of 1
int fixInterval = 1;			//Default value of 1
const char* logConf = NULL;
int introducerPort = -1;
const char* logConfig = NULL;

int main(int argc, char** argv){

	//Parse the args from the shell, initialize all globals
	parse_args(argc, argv);


	//Starts node0 if required
	setup_and_connect_to_node();
	
	//Loop indefinitely and ask for commands from user
	handle_command();

return 0;
}

//Used for redirecting apache warnings
void donothing(const char*){}

//Main function that handles commands form user
//ASSUMPTIONS: calling 'exit' will quit the program
void handle_command(){

	string commandinput;
	string command;

	string filename;
	string filedata;
	int getnodeid;
	while(1){

	//Parse the command
	cout<<"Enter command: ";
	getline(cin,commandinput);
	istringstream stream1(commandinput, istringstream::in);
	stream1 >> command;

			/* PArse the arguments */

			if(command == "ADD_NODE"){
				while(!stream1.eof()){
					stream1>>getnodeid;
					add_node(getnodeid);

				}
			}

			else if(command == "ADD_FILE"){
				stream1>>filename;
				stream1.get();
				getline(stream1,filedata); 
				add_file(filename, filedata);
			}

			else if(command == "DEL_FILE"){
				stream1>>filename;
				del_file(filename);
			}
			else if(command == "GET_FILE"){
				stream1>>filename;
				get_file(filename);
			}

			else if(command == "GET_TABLE"){
				stream1>>getnodeid;
				get_table(getnodeid);
			}
			else if(command == "exit"){
				cout<<"Exiting... "<<endl;
				exit(0);
			}
			else{
				cout<<"Invalid command"<<endl<<"Enter 'exit' to quit program\n";
			}
	}	
}

//called first time listener starts, to start node 0 (or attach to specific node)
void setup_and_connect_to_node(){

	srand(time(NULL));
	
	currentPort = startingPort;
	if(startingPort == -1){
		currentPort = (rand() % 7000)+3000;
	}

	if(attachToNode == -1){
		add_node(0);
	}
	
}

/* Creates a node process 
* @param id: the id of the node to be created
*	ASSUMPTIONS: Will never be called on a node already present
*/
void add_node(int id){

	bool try_again = true;
		//Loop until you connect to valid port
	while(try_again){
	
			char *exec_cmd[9];
			int buf_size = 30;		

			//for the node args
			for(int i=0;i<8;i++){
				exec_cmd[i] = new char[buf_size];
			}
			int count = 0;
			//create exec command 
			sprintf(exec_cmd[count++], "./listener");
	
			sprintf(exec_cmd[count++], "--m=%d",m);
			sprintf(exec_cmd[count++], "--id=%d",id);
			sprintf(exec_cmd[count++], "--port=%d",currentPort);
	
			//If creating node0 then initialize so it can be used later
			if(id==0)
				introducerPort = currentPort;
			//Send the introducer port to all other nodes being created	
			if(id!=0)
				if(introducerPort != -1){
					sprintf(exec_cmd[count++], "--introducerPort=%d",introducerPort);
				}
			if(stabilizeInterval != -1){
				sprintf(exec_cmd[count++], "--stabilizeInterval=%d",stabilizeInterval);
			}
			if(fixInterval != -1){
				sprintf(exec_cmd[count++], "--fixInterval=%d",fixInterval);
			}
			if(logConfig != NULL){
				sprintf(exec_cmd[count++], "--logConfig=%s",logConfig);
	
			}

			//Point the last arg to null
			exec_cmd[count] = NULL;

			//Fork, and then if child, immediately execv into node process code
			pid_t pid = fork();
			if(pid == 0){
				//Child precess, execv into node.cpp with parameters
				execv("./node", exec_cmd); 
			}
	
			//Try connecting to node
			sleep(1);
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", currentPort));
			boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
			boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

			try{
				MyServiceClient client(protocol);
				transport->open();
				transport->close();
				try_again = false;
				//cout<<"New Node id="<<id<<", port= "<<currentPort<<endl;
			}
			catch(apache::thrift::transport::TTransportException oops){
				//cout<<"------- ----------CAUGHT EXCEPTION IN LISTENER , relaunching-------\n";
				transport->close();
				try_again = true;			
			}
			if(currentPort > 9990){
				currentPort = 3500;
			}
			currentPort++;

	}	
}

/* 
 * Calls rpc on attached node to add a file
 * @param: filename: name of the file
 * @param filedata: the data contents of file
 */
void add_file(string filename, string filedata){
		
		int cur_port;
		if(attachToNode != -1) 
			cur_port = attachToNode;
		else
			cur_port = introducerPort;

		boost::shared_ptr<TSocket> socket(new TSocket("localhost", cur_port));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

		MyServiceClient client(protocol);
		transport->open();
		string result;
		//Calls rpc with key = -1, so the first node to see -1 will hash filename and then pass that along
		client.rpc_add_file(result, filename, filedata, -1);
		transport->close();
		cout<<result<<endl;

	}

/*
 * Get table for particular node
 * @param: node id of the requested table
 * ASSUMPTIONS: will be called only on a valid node. Because there's no way to check otherwise
 */ 
void get_table(int getnodeid){

		int cur_port;
		if(attachToNode != -1){
			cur_port = attachToNode;
		}
		else{
			cur_port = introducerPort;
		}

		boost::shared_ptr<TSocket> socket(new TSocket("localhost", cur_port));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

		MyServiceClient client(protocol);
		transport->open();
		string result;
		client.rpc_get_table(result, getnodeid);
		transport->close();
		cout<<result<<endl;
}

/*
 * Gets the information about the location of a file
 * @param filename: the file to be searched for 
 *  
 */
void get_file(string filename){
		
		int cur_port;
		if(attachToNode != -1){
			cur_port = attachToNode;
		}
		else{
			cur_port = introducerPort;
		}

		boost::shared_ptr<TSocket> socket(new TSocket("localhost", cur_port));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));


		MyServiceClient client(protocol);
		transport->open();
		string result;
		//Send in key  as -1, assuming that first node to see -1 will hash filename and pass it on
		client.rpc_get_file(result, filename, -1);
		transport->close();
		cout<<result<<endl;

}

/*
 * Deletes given file
 * @param filename: the file to be deleted
 * if file does not exist then warning message is printed out
 *  
 */

void del_file(string filename){

		int cur_port;
		if(attachToNode != -1){
			cur_port = attachToNode;
		}
		else{
			cur_port = introducerPort;
		}

		boost::shared_ptr<TSocket> socket(new TSocket("localhost", cur_port));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));


		MyServiceClient client(protocol);
		transport->open();
		string result;
		//Send in key  as -1, assuming that first node to see -1 will hash filename and pass it on
		client.rpc_del_file(result, filename, -1);
		transport->close();
		cout<<result<<endl;

}

/*
 * Parses argv
 * Inspired from example code given
 * @param: args given to main()
 */ 
void parse_args(int argc, char **argv){

    struct option long_options[] = {
        /* mandatory args */

        {"m", required_argument, 0, 1000},

        /* optional args */

        /* Start of sequence of ports the listener should try.
         */
        {"startingPort", required_argument, 0, 1001},

        /* port of which node to connect to, not not given, create node 0. */
        {"attachToNode", required_argument, 0, 1002},

		
        /* intervals (seconds) for runs of the stabilization and
         * fixfinger algorithms */
        {"stabilizeInterval", required_argument, 0, 1003},
        {"fixInterval", required_argument, 0, 1004},
		
		
		/*logging file*/
		{"logConf", required_argument, 0, 1005},

        {0, 0, 0, 0},
    };
	
	int opt;	
	int long_index;
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
            startingPort = strtol(optarg, NULL, 10);
            assert(startingPort >= 0);
            break;

        case 1002:
			attachToNode = strtol(optarg, NULL, 10);
            assert(attachToNode > 0);
            break;

        case 1003:
            stabilizeInterval = strtol(optarg, NULL, 10);
            assert(stabilizeInterval > 0);
            break;

        case 1004:
            fixInterval = strtol(optarg, NULL, 10);
            assert(fixInterval > 0);
            break;

        case 1005:
            logConf = optarg;
            break;

        default:
            exit(1);
        }
    }
}
