#include "binom/includes/binom.h"
#include "TcpServer/tcp/hdr/TcpServer.h"
#include "TcpServer/tcp/hdr/TcpClient.h"

#include <ctime>
#include <functional>

const char SEPARATOR[] = "=========================================================================\n\n";

using namespace binom;

std::string getHostStr(const TcpServer::Client& client) {
  uint32_t ip = client.getHost ();
  return std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
     std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
     std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
     std::to_string(int(reinterpret_cast<char*>(&ip)[3])) + ':' +
     std::to_string( client.getPort ());
}

BinOMDataBase db("db.binom", vobj{
                   {"some_messages", varr{
                      "first_message",
                      "second_message",
                      "third_message"
                    }}
                 });

TcpServer server(8080, [](TcpServer::Client client) {
  std::string client_addr_str = getHostStr(client);
  try {
    std::clog << "Server:\n|Client " << client_addr_str << " connected\n|Waiting data...\n";
    DataDescriptor descriptor = client.waitData();
    std::clog << "Server:\n|Recived data from: " << client_addr_str << '\n'
              << "|Size: " << descriptor.size << " bytes\n";
    ByteArray client_data(descriptor.data_ptr, descriptor.size);
    Path path = Path::fromByteArray(client_data);
    ByteArray result_data = db.getRoot()(path).getVariable().serialize();
    client.sendData(reinterpret_cast<char*>(result_data.begin()), result_data.length());
  } catch (Exception& except) {
    std::string err_msg = except.full();
    std::clog << "Server:\n|Client " << client_addr_str << " error:\n" << '|' << err_msg << '\n';
    ByteArray result_data = Variable(vobj{
                                       {"err", err_msg}
                                     }).serialize();
    client.sendData(reinterpret_cast<char*>(result_data.begin()), result_data.length());
  }
});

void testServer() {
  //Start server
  if(server.start() == TcpServer::status::up) {
    std::clog<<"Server:\n|Server is up!"<<std::endl;
//    server.joinLoop(); //Joing to the client handling loop
    return;
  } else {
    std::clog<<"Server:\n|Server start error! Error code:"<< int(server.getStatus()) <<std::endl;
    std::exit(-1);
  }
}


void testClient() {
  TcpClient client;
  if(client.connectTo(LOCALHOST_IP, 8080) == TcpClient::status::connected) {

    std::thread distant_msg([&client](){ // Server data reciver
      DataDescriptor descriptor = client.waitData();
      Variable data = Variable::deserialize(ByteArray(descriptor.data_ptr, descriptor.size));
      if(data.contains("err")) { // Error handling
        std::clog << "Client:\n|" << data.getVariable("err").toBufferArray().toString() << std::endl;
      } else std::clog << "Client:\n|Recived data from server: " << data << '\n';
    });

//    ByteArray send_data = Path({"some_messages",4}).toByteArray(); // With error
    ByteArray send_data = Path({"some_messages"}).toByteArray();
    client.sendData(reinterpret_cast<char*>(send_data.begin()), send_data.length());
    std::clog << "Client:\n|Client sent data\n";

    distant_msg.join();
    server.stop();

  } else {
    std::clog<<"Client:\n|Error! Client isn't connected! Error code: " << int(client.getStatus ()) << std::endl;
    server.stop();
    std::exit(-1);
  }
}


int main() {
  try {

    testServer();
    testClient();

    std::clog << "=========================================================================\n"
              << "|                    Test completed successfully                        |\n"
              << "=========================================================================\n";

    throw Exception(ErrCode::any, "Test");

  } catch(Exception& except) {
    std::cerr << except.full() << std::endl;
  } catch(std::exception& except) {
    std::cerr << except.what() << std::endl;
  }

  return 0;
}
