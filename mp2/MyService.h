/**
 * Autogenerated by Thrift Compiler (0.8.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef MyService_H
#define MyService_H

#include <TProcessor.h>
#include "mp2_types.h"

namespace mp2 {

class MyServiceIf {
 public:
  virtual ~MyServiceIf() {}
  virtual void rpc_find_successor(node_info& _return, const int32_t key) = 0;
  virtual void rpc_give_local_successor(node_info& _return) = 0;
  virtual void find_predecessor(node_info& _return, const int32_t key) = 0;
  virtual void rpc_return_finger_table(std::vector<node_info> & _return) = 0;
  virtual void rpc_return_key_table(std::map<int32_t, file_info> & _return) = 0;
  virtual void rpc_return_predecessor(node_info& _return) = 0;
  virtual void rpc_notify_of_predecessor(const node_info& new_pre) = 0;
};

class MyServiceIfFactory {
 public:
  typedef MyServiceIf Handler;

  virtual ~MyServiceIfFactory() {}

  virtual MyServiceIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(MyServiceIf* /* handler */) = 0;
};

class MyServiceIfSingletonFactory : virtual public MyServiceIfFactory {
 public:
  MyServiceIfSingletonFactory(const boost::shared_ptr<MyServiceIf>& iface) : iface_(iface) {}
  virtual ~MyServiceIfSingletonFactory() {}

  virtual MyServiceIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(MyServiceIf* /* handler */) {}

 protected:
  boost::shared_ptr<MyServiceIf> iface_;
};

class MyServiceNull : virtual public MyServiceIf {
 public:
  virtual ~MyServiceNull() {}
  void rpc_find_successor(node_info& /* _return */, const int32_t /* key */) {
    return;
  }
  void rpc_give_local_successor(node_info& /* _return */) {
    return;
  }
  void find_predecessor(node_info& /* _return */, const int32_t /* key */) {
    return;
  }
  void rpc_return_finger_table(std::vector<node_info> & /* _return */) {
    return;
  }
  void rpc_return_key_table(std::map<int32_t, file_info> & /* _return */) {
    return;
  }
  void rpc_return_predecessor(node_info& /* _return */) {
    return;
  }
  void rpc_notify_of_predecessor(const node_info& /* new_pre */) {
    return;
  }
};

typedef struct _MyService_rpc_find_successor_args__isset {
  _MyService_rpc_find_successor_args__isset() : key(false) {}
  bool key;
} _MyService_rpc_find_successor_args__isset;

class MyService_rpc_find_successor_args {
 public:

  MyService_rpc_find_successor_args() : key(0) {
  }

  virtual ~MyService_rpc_find_successor_args() throw() {}

  int32_t key;

  _MyService_rpc_find_successor_args__isset __isset;

  void __set_key(const int32_t val) {
    key = val;
  }

  bool operator == (const MyService_rpc_find_successor_args & rhs) const
  {
    if (!(key == rhs.key))
      return false;
    return true;
  }
  bool operator != (const MyService_rpc_find_successor_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MyService_rpc_find_successor_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class MyService_rpc_find_successor_pargs {
 public:


  virtual ~MyService_rpc_find_successor_pargs() throw() {}

  const int32_t* key;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _MyService_rpc_find_successor_result__isset {
  _MyService_rpc_find_successor_result__isset() : success(false) {}
  bool success;
} _MyService_rpc_find_successor_result__isset;

class MyService_rpc_find_successor_result {
 public:

  MyService_rpc_find_successor_result() {
  }

  virtual ~MyService_rpc_find_successor_result() throw() {}

  node_info success;

  _MyService_rpc_find_successor_result__isset __isset;

  void __set_success(const node_info& val) {
    success = val;
  }

  bool operator == (const MyService_rpc_find_successor_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const MyService_rpc_find_successor_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MyService_rpc_find_successor_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _MyService_rpc_find_successor_presult__isset {
  _MyService_rpc_find_successor_presult__isset() : success(false) {}
  bool success;
} _MyService_rpc_find_successor_presult__isset;

class MyService_rpc_find_successor_presult {
 public:


  virtual ~MyService_rpc_find_successor_presult() throw() {}

  node_info* success;

  _MyService_rpc_find_successor_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};


class MyService_rpc_give_local_successor_args {
 public:

  MyService_rpc_give_local_successor_args() {
  }

  virtual ~MyService_rpc_give_local_successor_args() throw() {}


  bool operator == (const MyService_rpc_give_local_successor_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const MyService_rpc_give_local_successor_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MyService_rpc_give_local_successor_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class MyService_rpc_give_local_successor_pargs {
 public:


  virtual ~MyService_rpc_give_local_successor_pargs() throw() {}


  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _MyService_rpc_give_local_successor_result__isset {
  _MyService_rpc_give_local_successor_result__isset() : success(false) {}
  bool success;
} _MyService_rpc_give_local_successor_result__isset;

class MyService_rpc_give_local_successor_result {
 public:

  MyService_rpc_give_local_successor_result() {
  }

  virtual ~MyService_rpc_give_local_successor_result() throw() {}

  node_info success;

  _MyService_rpc_give_local_successor_result__isset __isset;

  void __set_success(const node_info& val) {
    success = val;
  }

  bool operator == (const MyService_rpc_give_local_successor_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const MyService_rpc_give_local_successor_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MyService_rpc_give_local_successor_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _MyService_rpc_give_local_successor_presult__isset {
  _MyService_rpc_give_local_successor_presult__isset() : success(false) {}
  bool success;
} _MyService_rpc_give_local_successor_presult__isset;

class MyService_rpc_give_local_successor_presult {
 public:


  virtual ~MyService_rpc_give_local_successor_presult() throw() {}

  node_info* success;

  _MyService_rpc_give_local_successor_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _MyService_find_predecessor_args__isset {
  _MyService_find_predecessor_args__isset() : key(false) {}
  bool key;
} _MyService_find_predecessor_args__isset;

class MyService_find_predecessor_args {
 public:

  MyService_find_predecessor_args() : key(0) {
  }

  virtual ~MyService_find_predecessor_args() throw() {}

  int32_t key;

  _MyService_find_predecessor_args__isset __isset;

  void __set_key(const int32_t val) {
    key = val;
  }

  bool operator == (const MyService_find_predecessor_args & rhs) const
  {
    if (!(key == rhs.key))
      return false;
    return true;
  }
  bool operator != (const MyService_find_predecessor_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MyService_find_predecessor_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class MyService_find_predecessor_pargs {
 public:


  virtual ~MyService_find_predecessor_pargs() throw() {}

  const int32_t* key;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _MyService_find_predecessor_result__isset {
  _MyService_find_predecessor_result__isset() : success(false) {}
  bool success;
} _MyService_find_predecessor_result__isset;

class MyService_find_predecessor_result {
 public:

  MyService_find_predecessor_result() {
  }

  virtual ~MyService_find_predecessor_result() throw() {}

  node_info success;

  _MyService_find_predecessor_result__isset __isset;

  void __set_success(const node_info& val) {
    success = val;
  }

  bool operator == (const MyService_find_predecessor_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const MyService_find_predecessor_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MyService_find_predecessor_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _MyService_find_predecessor_presult__isset {
  _MyService_find_predecessor_presult__isset() : success(false) {}
  bool success;
} _MyService_find_predecessor_presult__isset;

class MyService_find_predecessor_presult {
 public:


  virtual ~MyService_find_predecessor_presult() throw() {}

  node_info* success;

  _MyService_find_predecessor_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};


class MyService_rpc_return_finger_table_args {
 public:

  MyService_rpc_return_finger_table_args() {
  }

  virtual ~MyService_rpc_return_finger_table_args() throw() {}


  bool operator == (const MyService_rpc_return_finger_table_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const MyService_rpc_return_finger_table_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MyService_rpc_return_finger_table_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class MyService_rpc_return_finger_table_pargs {
 public:


  virtual ~MyService_rpc_return_finger_table_pargs() throw() {}


  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _MyService_rpc_return_finger_table_result__isset {
  _MyService_rpc_return_finger_table_result__isset() : success(false) {}
  bool success;
} _MyService_rpc_return_finger_table_result__isset;

class MyService_rpc_return_finger_table_result {
 public:

  MyService_rpc_return_finger_table_result() {
  }

  virtual ~MyService_rpc_return_finger_table_result() throw() {}

  std::vector<node_info>  success;

  _MyService_rpc_return_finger_table_result__isset __isset;

  void __set_success(const std::vector<node_info> & val) {
    success = val;
  }

  bool operator == (const MyService_rpc_return_finger_table_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const MyService_rpc_return_finger_table_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MyService_rpc_return_finger_table_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _MyService_rpc_return_finger_table_presult__isset {
  _MyService_rpc_return_finger_table_presult__isset() : success(false) {}
  bool success;
} _MyService_rpc_return_finger_table_presult__isset;

class MyService_rpc_return_finger_table_presult {
 public:


  virtual ~MyService_rpc_return_finger_table_presult() throw() {}

  std::vector<node_info> * success;

  _MyService_rpc_return_finger_table_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};


class MyService_rpc_return_key_table_args {
 public:

  MyService_rpc_return_key_table_args() {
  }

  virtual ~MyService_rpc_return_key_table_args() throw() {}


  bool operator == (const MyService_rpc_return_key_table_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const MyService_rpc_return_key_table_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MyService_rpc_return_key_table_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class MyService_rpc_return_key_table_pargs {
 public:


  virtual ~MyService_rpc_return_key_table_pargs() throw() {}


  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _MyService_rpc_return_key_table_result__isset {
  _MyService_rpc_return_key_table_result__isset() : success(false) {}
  bool success;
} _MyService_rpc_return_key_table_result__isset;

class MyService_rpc_return_key_table_result {
 public:

  MyService_rpc_return_key_table_result() {
  }

  virtual ~MyService_rpc_return_key_table_result() throw() {}

  std::map<int32_t, file_info>  success;

  _MyService_rpc_return_key_table_result__isset __isset;

  void __set_success(const std::map<int32_t, file_info> & val) {
    success = val;
  }

  bool operator == (const MyService_rpc_return_key_table_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const MyService_rpc_return_key_table_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MyService_rpc_return_key_table_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _MyService_rpc_return_key_table_presult__isset {
  _MyService_rpc_return_key_table_presult__isset() : success(false) {}
  bool success;
} _MyService_rpc_return_key_table_presult__isset;

class MyService_rpc_return_key_table_presult {
 public:


  virtual ~MyService_rpc_return_key_table_presult() throw() {}

  std::map<int32_t, file_info> * success;

  _MyService_rpc_return_key_table_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};


class MyService_rpc_return_predecessor_args {
 public:

  MyService_rpc_return_predecessor_args() {
  }

  virtual ~MyService_rpc_return_predecessor_args() throw() {}


  bool operator == (const MyService_rpc_return_predecessor_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const MyService_rpc_return_predecessor_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MyService_rpc_return_predecessor_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class MyService_rpc_return_predecessor_pargs {
 public:


  virtual ~MyService_rpc_return_predecessor_pargs() throw() {}


  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _MyService_rpc_return_predecessor_result__isset {
  _MyService_rpc_return_predecessor_result__isset() : success(false) {}
  bool success;
} _MyService_rpc_return_predecessor_result__isset;

class MyService_rpc_return_predecessor_result {
 public:

  MyService_rpc_return_predecessor_result() {
  }

  virtual ~MyService_rpc_return_predecessor_result() throw() {}

  node_info success;

  _MyService_rpc_return_predecessor_result__isset __isset;

  void __set_success(const node_info& val) {
    success = val;
  }

  bool operator == (const MyService_rpc_return_predecessor_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const MyService_rpc_return_predecessor_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MyService_rpc_return_predecessor_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _MyService_rpc_return_predecessor_presult__isset {
  _MyService_rpc_return_predecessor_presult__isset() : success(false) {}
  bool success;
} _MyService_rpc_return_predecessor_presult__isset;

class MyService_rpc_return_predecessor_presult {
 public:


  virtual ~MyService_rpc_return_predecessor_presult() throw() {}

  node_info* success;

  _MyService_rpc_return_predecessor_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _MyService_rpc_notify_of_predecessor_args__isset {
  _MyService_rpc_notify_of_predecessor_args__isset() : new_pre(false) {}
  bool new_pre;
} _MyService_rpc_notify_of_predecessor_args__isset;

class MyService_rpc_notify_of_predecessor_args {
 public:

  MyService_rpc_notify_of_predecessor_args() {
  }

  virtual ~MyService_rpc_notify_of_predecessor_args() throw() {}

  node_info new_pre;

  _MyService_rpc_notify_of_predecessor_args__isset __isset;

  void __set_new_pre(const node_info& val) {
    new_pre = val;
  }

  bool operator == (const MyService_rpc_notify_of_predecessor_args & rhs) const
  {
    if (!(new_pre == rhs.new_pre))
      return false;
    return true;
  }
  bool operator != (const MyService_rpc_notify_of_predecessor_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MyService_rpc_notify_of_predecessor_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class MyService_rpc_notify_of_predecessor_pargs {
 public:


  virtual ~MyService_rpc_notify_of_predecessor_pargs() throw() {}

  const node_info* new_pre;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class MyService_rpc_notify_of_predecessor_result {
 public:

  MyService_rpc_notify_of_predecessor_result() {
  }

  virtual ~MyService_rpc_notify_of_predecessor_result() throw() {}


  bool operator == (const MyService_rpc_notify_of_predecessor_result & /* rhs */) const
  {
    return true;
  }
  bool operator != (const MyService_rpc_notify_of_predecessor_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const MyService_rpc_notify_of_predecessor_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class MyService_rpc_notify_of_predecessor_presult {
 public:


  virtual ~MyService_rpc_notify_of_predecessor_presult() throw() {}


  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class MyServiceClient : virtual public MyServiceIf {
 public:
  MyServiceClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  MyServiceClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void rpc_find_successor(node_info& _return, const int32_t key);
  void send_rpc_find_successor(const int32_t key);
  void recv_rpc_find_successor(node_info& _return);
  void rpc_give_local_successor(node_info& _return);
  void send_rpc_give_local_successor();
  void recv_rpc_give_local_successor(node_info& _return);
  void find_predecessor(node_info& _return, const int32_t key);
  void send_find_predecessor(const int32_t key);
  void recv_find_predecessor(node_info& _return);
  void rpc_return_finger_table(std::vector<node_info> & _return);
  void send_rpc_return_finger_table();
  void recv_rpc_return_finger_table(std::vector<node_info> & _return);
  void rpc_return_key_table(std::map<int32_t, file_info> & _return);
  void send_rpc_return_key_table();
  void recv_rpc_return_key_table(std::map<int32_t, file_info> & _return);
  void rpc_return_predecessor(node_info& _return);
  void send_rpc_return_predecessor();
  void recv_rpc_return_predecessor(node_info& _return);
  void rpc_notify_of_predecessor(const node_info& new_pre);
  void send_rpc_notify_of_predecessor(const node_info& new_pre);
  void recv_rpc_notify_of_predecessor();
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class MyServiceProcessor : public ::apache::thrift::TProcessor {
 protected:
  boost::shared_ptr<MyServiceIf> iface_;
  virtual bool process_fn(apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, std::string& fname, int32_t seqid, void* callContext);
 private:
  std::map<std::string, void (MyServiceProcessor::*)(int32_t, apache::thrift::protocol::TProtocol*, apache::thrift::protocol::TProtocol*, void*)> processMap_;
  void process_rpc_find_successor(int32_t seqid, apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_rpc_give_local_successor(int32_t seqid, apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_find_predecessor(int32_t seqid, apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_rpc_return_finger_table(int32_t seqid, apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_rpc_return_key_table(int32_t seqid, apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_rpc_return_predecessor(int32_t seqid, apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_rpc_notify_of_predecessor(int32_t seqid, apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  MyServiceProcessor(boost::shared_ptr<MyServiceIf> iface) :
    iface_(iface) {
    processMap_["rpc_find_successor"] = &MyServiceProcessor::process_rpc_find_successor;
    processMap_["rpc_give_local_successor"] = &MyServiceProcessor::process_rpc_give_local_successor;
    processMap_["find_predecessor"] = &MyServiceProcessor::process_find_predecessor;
    processMap_["rpc_return_finger_table"] = &MyServiceProcessor::process_rpc_return_finger_table;
    processMap_["rpc_return_key_table"] = &MyServiceProcessor::process_rpc_return_key_table;
    processMap_["rpc_return_predecessor"] = &MyServiceProcessor::process_rpc_return_predecessor;
    processMap_["rpc_notify_of_predecessor"] = &MyServiceProcessor::process_rpc_notify_of_predecessor;
  }

  virtual bool process(boost::shared_ptr<apache::thrift::protocol::TProtocol> piprot, boost::shared_ptr<apache::thrift::protocol::TProtocol> poprot, void* callContext);
  virtual ~MyServiceProcessor() {}
};

class MyServiceProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  MyServiceProcessorFactory(const ::boost::shared_ptr< MyServiceIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< MyServiceIfFactory > handlerFactory_;
};

class MyServiceMultiface : virtual public MyServiceIf {
 public:
  MyServiceMultiface(std::vector<boost::shared_ptr<MyServiceIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~MyServiceMultiface() {}
 protected:
  std::vector<boost::shared_ptr<MyServiceIf> > ifaces_;
  MyServiceMultiface() {}
  void add(boost::shared_ptr<MyServiceIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void rpc_find_successor(node_info& _return, const int32_t key) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        ifaces_[i]->rpc_find_successor(_return, key);
        return;
      } else {
        ifaces_[i]->rpc_find_successor(_return, key);
      }
    }
  }

  void rpc_give_local_successor(node_info& _return) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        ifaces_[i]->rpc_give_local_successor(_return);
        return;
      } else {
        ifaces_[i]->rpc_give_local_successor(_return);
      }
    }
  }

  void find_predecessor(node_info& _return, const int32_t key) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        ifaces_[i]->find_predecessor(_return, key);
        return;
      } else {
        ifaces_[i]->find_predecessor(_return, key);
      }
    }
  }

  void rpc_return_finger_table(std::vector<node_info> & _return) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        ifaces_[i]->rpc_return_finger_table(_return);
        return;
      } else {
        ifaces_[i]->rpc_return_finger_table(_return);
      }
    }
  }

  void rpc_return_key_table(std::map<int32_t, file_info> & _return) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        ifaces_[i]->rpc_return_key_table(_return);
        return;
      } else {
        ifaces_[i]->rpc_return_key_table(_return);
      }
    }
  }

  void rpc_return_predecessor(node_info& _return) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        ifaces_[i]->rpc_return_predecessor(_return);
        return;
      } else {
        ifaces_[i]->rpc_return_predecessor(_return);
      }
    }
  }

  void rpc_notify_of_predecessor(const node_info& new_pre) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      ifaces_[i]->rpc_notify_of_predecessor(new_pre);
    }
  }

};

} // namespace

#endif
