namespace cpp mp2

struct node_info{
	1: i32 id;
	2: i32 port;
}

struct file_info{
	1:string name;
	2:string data;
}

/* declare the RPC interface of your network service */
service MyService {

	//Other node asked me to find successor for 'key'
	node_info rpc_find_successor(1:i32 key);

	//Other node asked for my own successor
	node_info rpc_give_local_successor();

	//helper function for finding closest predecessor
	node_info find_predecessor(1:i32 key);

	//Node asks for my finger table (eg when joining)
	list<node_info> rpc_return_finger_table();

	//Node asks for my key table
	map<i32, file_info> rpc_return_key_table();

	node_info rpc_return_predecessor();

	void rpc_notify_of_predecessor(1:node_info new_pre);

}
