namespace cpp mp2

struct node_info{
	1: i32 id;
	2: i32 port;
}

/* declare the RPC interface of your network service */
service MyService {

	//Other node asked me to find successor for 'key'
	node_info rpc_find_successor(1:i32 key);

	//Other node asked for my own successor
	node_info rpc_give_local_successor();

	//helper function for finding closest predecessor
	node_info find_predecessor(1:i32 key);

}
