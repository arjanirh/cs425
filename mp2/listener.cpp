#include <iostream>
#include <stdio.h>
#include <string>
#include <getopt.h>
#include <stdlib.h>
#include <assert.h>
#include <sstream.h>
#include <time.h>
#include <stdlib.h>


#include "MyService.h"
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>


using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

// the name space specified in the thrift file
using namespace mp2;

using namespace std;

struct node_info{

	int id;
	int port;
};

//Functions declarations
void parse_args(int argc, char **argv);
void setup_and_connect_to_node();


//Global variables
int m=-1;
int startingPort = -1;
int currentPort = -1;
int attachToNode = -1;
int stabilizeInterval = 1;		//Default value of 1
int fixInterval = 1;			//Default value of 1
const char* logConf = NULL;
int introducerPort = -1;


int main(int argc, char** argv){

	parse_args(argc, argv);

	setup_and_connect_to_node();


	while(1){
		//receive command from user
		cout<<"Enter Command: ";

		//If command = ADD_NODE then start a new node process gicing it required parameters
		//cin parsing
		//else
		//Send command to connected node, and receive response string, display to user
	
	}

return 0;
}

void setup_and_connect_to_node(){

	srand(time(NULL));

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
		boost::shared_ptr<TSocket> socket(new TSocket("localhost", introducerPort));
		//boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		//boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
		
		//if cant connect on port, then try another port

	}
	else{
		//connect to that node
	}


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
