#ifndef BINOM_TCP_IO_H
#define BINOM_TCP_IO_H

#include "binom/includes/binom.h"
#include "TcpServer/tcp/hdr/TcpServer.h"
#include "TcpServer/tcp/hdr/TcpClient.h"

using namespace binom;


//// Tcp IO-streams

DataBuffer& operator >> (DataBuffer& data, ByteArray& array) {
  array.pushBack(data.data_ptr, data.size);
  return data;
}

TcpServer::Client& operator << (TcpServer::Client& client, Variable& var) {
  ByteArray data = var.serialize();
  client.sendData((char*)data.begin(), data.length());
  return client;
}

BinOMDataBase data_base("db.binomdb", vobj{
                          {"users", varr{
                             vobj{
                               {"login", "admin"},
                               {"password", "admin"}
                             }
                           }}
});

DBNodeVisitor root = data_base.getRoot();

TcpServer server(8080,
[](DataBuffer data, TcpServer::Client& client)->void{
  try {
    ByteArray array;
    data >> array;

    Path path = Path::fromByteArray(array);
    Variable var = root[path].getVariable();
    client << var;

  }  catch (Exception& except) {

    Variable err_var = vobj{
      {"err_code", ui8(except.code())},
      {"err_string", except.full()}
    };
    client << err_var;

  }
}, {120,3,5});

bool testTcpServer() {
  if(server.start() == TcpServer::status::up) {
    return true;
  } else {
    return false;
  }
}


void testTcpClient() {
  TcpClient client;
  client.connectTo(LOCALHOST_IP, 8080);
  ByteArray pth_data = Path{"users", 0}.toByteArray();
  client.sendData(pth_data.begin(), pth_data.length());
  DataBuffer var_data = client.loadData();
  ByteArray var_arr;
  var_arr.pushBack(var_data.data_ptr, var_data.size);
  Variable var = Variable::deserialize(var_arr);
  std::cout << var;
}

void test() {
  if(testTcpServer()) {
    using namespace std::chrono_literals;
    for(int i = 0; i < 1000; ++i) {
      std::thread thr1(testTcpClient);
      thr1.detach();
    }
//    printf("Awaiting...");
    std::this_thread::sleep_for(10s);
    server.stop();
  } else std::exit(-1);
}


#endif // BINOM_TCP_IO_H
