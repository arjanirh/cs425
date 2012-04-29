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

//Added
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::mp2;

using namespace std;

struct node_info{

	int id;
	int port;
};

//Functions declarations
void parse_args(int argc, char **argv);
void setup_and_connect_to_node();
void handle_command();
void add_file();
void del_file();
void get_file();
void get_table();
void add_node(int id);



//Global variables
int m=-1;
int startingPort = -1;
int currentPort = -1;
int attachToNode = -1;
int stabilizeInterval = 1;		//Default value of 1
int fixInterval = 1;			//Default value of 1
const char* logConf = NULL;
int introducerPort = -1;
string filename;
string filedata;
int getnodeid;
const char* logConfig = NULL;

int main(int argc, char** argv){

	parse_args(argc, argv);

//	setup_and_connect_to_node();
	
	handle_command();

return 0;
}

void handle_command(){

	string commandinput;
	string command;

	while(1){

	cout<<"Enter command"<<endl;
	getline(cin,commandinput);
	istringstream stream1(commandinput, istringstream::in);
	stream1 >> command;

			if(command == "ADD_NODE"){
			cout<<"Add node was called\n";
			}

			else if(command == "ADD_FILE"){
				stream1>>filename;
				stream1.get();
				getline(stream1,filedata); 
				//add_file();
			}

			//case 'DEL_FILE':
			else if(command == "DEL_FILE"){
				stream1>>filename;
//				cout<<"delete file was called with filename: "<<filename<<endl; 
				//del_file();
			}
			//case 'GET_FILE':
			else if(command == "GET_FILE"){
				stream1>>filename;
//				cout<<"get file was called with filename: "<<filename<<endl; 
				//get_file();
			}

			//case 'GET_TABLE':
			else if(command == "GET_TABLE"){
				stream1>>getnodeid;
//				cout<<"get Table was called with nodeid:"<<getnodeid<<endl;
				//get_table();
			}
			//case 'exit':
			else if(command == "exit"){
				cout<<"Exiting... "<<endl;
				exit(0);
			}

		//	default:
			else{
				cout<<"Invalid command"<<endl<<"Enter 'exit' to quit program\n";

			}
	}	
}

void setup_and_connect_to_node(){


	srand(time(NULL));
	
	if(attachToNode == -1){
		add_node(0);
	}
	
	boost::shared_ptr<TSocket> socket(new TSocket("localhost", introducerPort));
	//TODO: check if port was valid if socket == null

	return;
/*
	currentPort = startingPort;
	if(startingPort == -1){
		currentPort = (rand() % 8000)+1025;
	}

	if(attachToNode == -1){
		//create node 0 process 
		std::stringstream s;
		s << "./node";
		s<< " --m="<<m;
		s<< " --id="<<0;
		s<< " --port="<<currentPort;
		introducerPort = currentPort;
		currentPort++;

		if(stabilizeInterval != -1){
			s<< " --stabilizeInterval="<<stabilizeInterval;
		}
		if(fixInterval != -1){
			s<< " --fixInterval="<<fixInterval;
		}
		if(logConfig != NULL){
			s<< " --logConfig="<<logConfig;
		}
		
		string shell_cmd = s.str();
		system(&shell_cmd[0]);

		//connect to it (using thrift)
		//boost::shared_ptr<TSocket> socket(new TSocket("localhost", introducerPort));
		//boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		//boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
		
		//if cant connect on port, then try another port

	}
	else{
		//connect to that node
	}

*/
}

void add_node(int id){

	srand(time(NULL));

	currentPort = startingPort;
	if(startingPort == -1){
		currentPort = (rand() % 8000)+1025;
	}

	//if(attachToNode == -1){
		//create node 0 process 
		std::stringstream s;
		s << "./node";
		s<< " --m="<<m;
		s<< " --id="<<id;
		s<< " --port="<<currentPort;
		if(id==0)
			introducerPort = currentPort;
		currentPort++;

		if(id!=0)
			if(introducerPort != -1){
			s<< " --introducerPort="<<introducerPort;
			}
		if(stabilizeInterval != -1){
			s<< " --stabilizeInterval="<<stabilizeInterval;
		}
		if(fixInterval != -1){
			s<< " --fixInterval="<<fixInterval;
		}
		if(logConfig != NULL){
			s<< " --logConfig="<<logConfig;
		}

		string shell_cmd = s.str();
		system(&shell_cmd[0]);

		//connect to it (using thrift)
		boost::shared_ptr<TSocket> socket(new TSocket("localhost", introducerPort));
		//boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		//boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
}

void add_file(){
		
		int cur_port;
		if(attachToNode == -1) 
			cur_port = attachToNode;
			//boost::shared_ptr<TSocket> socket(new TSocket("localhost", attachToNode));
		else
			cur_port = introducerPort;
			//boost::shared_ptr<TSocket> socket(new TSocket("localhost", introducerPort));
		//boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		//boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

		boost::shared_ptr<TSocket> socket(new TSocket("localhost", cur_port));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

		MyServiceClient client(protocol);
		transport->open();
		string result;
		client.rpc_add_file(result, filename, filedata, -1);
		transport->close();
		cout<<result<<endl;

	}

void get_table(){

		int cur_port;
		if(attachToNode == -1){
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


void get_file(){
		
		/*
		if(attachToNode == -1) 
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", attachToNode));
		else
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", introducerPort));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
*/

		int cur_port;
		if(attachToNode == -1){
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
		client.rpc_get_file(result, filename, -1);
		transport->close();
		cout<<result<<endl;

	}


void del_file(){

		int cur_port;
		if(attachToNode == -1){
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
		client.rpc_del_file(result, filename, -1);
		transport->close();
		cout<<result<<endl;

}


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
