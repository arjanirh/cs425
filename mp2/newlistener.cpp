


Handle_Command(){
	string commandinput;
	string command;

	while(1){


		cout<<"Enter command"<<endl;
		cin<<getline (cin,commandinput);
		istringstream stream1(commandinput);
		commandinput>>command;
		
		switch(&command[0])
		{
			case 'ADD_NODE':

			case 'ADD_FILE':
				commandinput>>filename;
				commandinput>>filedata;
				ADD_FILE();
				break;
			case 'DEL_FILE':
				commandinput>>filename;
				DEL_FILE();
				break;
			case 'GET_FILE':
				commandinput>>filename;
				GET_FILE();
				break;
			case 'GET_TABLE':
				commandinput>>getnodeid;
				GET_TABLE();
				break;
			default:
				cout<<"Invalid command"<<endl;
				break;
	}
		



void ADD_NODE(int id){

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


void ADD_FILE(){
		
		if(attachToNode == -1) 
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", attachToNode));
		else
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", introducerPort));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

		MyServiceClient client(protocol);
		transport->open();
		string result;
		client.rpc_add_file(result, filename, filedata, -1);
		transport->close();
		cout<<result<<endl;

	}

void GET_TABLE(){

		if(attachToNode == -1) 
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", attachToNode));
		else
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", introducerPort));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

		MyServiceClient client(protocol);
		transport->open();
		string result;
		client.rpc_get_table(result, getnodeid);
		transport->close();
		cout<<result<<endl;

	}


void GET_FILE(){
		if(attachToNode == -1) 
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", attachToNode));
		else
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", introducerPort));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

		MyServiceClient client(protocol);
		transport->open();
		string result;
		client.rpc_get_file(result, filename, -1);
		transport->close();
		cout<<result<<endl;

	}


void DEL_FILE(){

		if(attachToNode == -1) 
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", attachToNode));
		else
			boost::shared_ptr<TSocket> socket(new TSocket("localhost", introducerPort));
		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

		MyServiceClient client(protocol);
		transport->open();
		string result;
		client.rpc_del_file(result, filename, -1);
		transport->close();
		cout<<result<<endl;

	}