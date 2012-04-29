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
void add_file(string, string);
void del_file(string );
void get_file(string);
void get_table(int);
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
const char* logConfig = NULL;

int main(int argc, char** argv){

	parse_args(argc, argv);

	setup_and_connect_to_node();
	
	handle_command();

return 0;
}

void handle_command(){

	string commandinput;
	string command;

	string filename;
	string filedata;
	int getnodeid;
	while(1){

	cout<<"Enter command: ";
	getline(cin,commandinput);
	istringstream stream1(commandinput, istringstream::in);
	stream1 >> command;

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

void setup_and_connect_to_node(){

	srand(time(NULL));
	
	if(attachToNode == -1){
		add_node(0);
	}
	
}

void add_node(int id){

	bool try_again = true;
		currentPort = startingPort;
		if(startingPort == -1){
			currentPort = (rand() % 8000)+1025;
		}
	while(try_again){
	
	
			char *exec_cmd[9];
			int buf_size = 30;		

			for(int i=0;i<8;i++){
				exec_cmd[i] = new char[buf_size];
			}
			int count = 0;
			sprintf(exec_cmd[count++], "./listener");
	
			sprintf(exec_cmd[count++], "--m=%d",m);
			sprintf(exec_cmd[count++], "--id=%d",id);
			sprintf(exec_cmd[count++], "--port=%d",currentPort);
	
			if(id==0)
				introducerPort = currentPort;
	
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
	
			exec_cmd[count] = NULL;
			pid_t pid = fork();
			if(pid == 0){
				//Child precess, execv into node
				execv("./node", exec_cmd); 
			}
	
			//Try connecting to node
			sleep(1);
			try{
				boost::shared_ptr<TSocket> socket(new TSocket("localhost", currentPort));
				boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
				boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
				
				MyServiceClient client(protocol);
				transport->open();
				transport->close();
				try_again = false;
			}
			catch(apache::thrift::transport::TTransportException oops){
				cout<<"Caught exception in listener\n";
				try_again = true;			
			}
			currentPort = (rand() % 8000)+1025;
	}	
}

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
		client.rpc_add_file(result, filename, filedata, -1);
		transport->close();
		cout<<result<<endl;

	}

void get_table(int getnodeid){

		
		int cur_port;
		if(attachToNode != -1){
			cur_port = attachToNode;
		}
		else{
			cur_port = introducerPort;
		}

		//cout<<"Getting table from port="<<cur_port<<"\n";
		boost::shared_ptr<TSocket> socket(new TSocket("localhost", cur_port));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

		//cout<<"b2\n";
		MyServiceClient client(protocol);
		transport->open();
		string result;
		client.rpc_get_table(result, getnodeid);
		transport->close();
		cout<<result<<endl;
		//cout<<"b3\n";

	}


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
		client.rpc_get_file(result, filename, -1);
		transport->close();
		cout<<result<<endl;

	}


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
