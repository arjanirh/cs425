namespace cpp mp2

struct suc_data{
	1: i32 id;
	2: i32 port;
}

/* declare the RPC interface of your network service */
service MyService {

	//Other node asked me to find successor for 'key'
	suc_data rpc_find_successor2(1:i32 key);

	//Other node asked for my own successor
	suc_data rpc_give_local_successor();

}
