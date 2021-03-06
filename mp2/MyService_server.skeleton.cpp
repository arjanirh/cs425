// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "MyService.h"
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::mp2;

class MyServiceHandler : virtual public MyServiceIf {
 public:
  MyServiceHandler() {
    // Your initialization goes here
  }

  void rpc_find_successor(node_info& _return, const int32_t key) {
    // Your implementation goes here
    printf("rpc_find_successor\n");
  }

  void rpc_give_local_successor(node_info& _return) {
    // Your implementation goes here
    printf("rpc_give_local_successor\n");
  }

  void find_predecessor(node_info& _return, const int32_t key) {
    // Your implementation goes here
    printf("find_predecessor\n");
  }

  void rpc_return_finger_table(std::vector<node_info> & _return) {
    // Your implementation goes here
    printf("rpc_return_finger_table\n");
  }

  void rpc_return_key_table(std::map<int32_t, file_info> & _return) {
    // Your implementation goes here
    printf("rpc_return_key_table\n");
  }

  void rpc_return_predecessor(node_info& _return) {
    // Your implementation goes here
    printf("rpc_return_predecessor\n");
  }

  void rpc_notify_of_predecessor(const node_info& new_pre) {
    // Your implementation goes here
    printf("rpc_notify_of_predecessor\n");
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<MyServiceHandler> handler(new MyServiceHandler());
  shared_ptr<TProcessor> processor(new MyServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

