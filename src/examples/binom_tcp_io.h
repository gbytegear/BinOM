#ifndef BINOM_TCP_IO_H
#define BINOM_TCP_IO_H

#include "binom/includes/binom.h"
#include "TcpServer/tcp/hdr/TcpServer.h"
#include "TcpServer/tcp/hdr/TcpClient.h"

using namespace binom;


//// Tcp IO-streams

//TcpClient& operator<<(TcpClient& client, ByteArray data) {
//  client.sendData(reinterpret_cast<char*>(data.begin()), data.length());
//  return client;
//}

//TcpClient& operator>>(TcpClient& client, ByteArray& data) {
//  DataDescriptor descriptor = client.waitData();
//  data = ByteArray(descriptor.data_ptr, descriptor.size);
//  return client;
//}

//TcpServer::Client& operator<<(TcpServer::Client& client, ByteArray data) {
//  client.sendData(reinterpret_cast<char*>(data.begin()), data.length());
//  return client;
//}

//TcpServer::Client& operator>>(TcpServer::Client& client, ByteArray& data) {
//  DataDescriptor descriptor = client.waitData();
//  data = ByteArray(descriptor.data_ptr, descriptor.size);
//  return client;
//}


//std::string getHostStr(const TcpServer::Client& client) {
//  uint32_t ip = client.getHost ();
//  return std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
//     std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
//     std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
//     std::to_string(int(reinterpret_cast<char*>(&ip)[3])) + ':' +
//     std::to_string( client.getPort ());
//}


//// Example of database
//BinOMDataBase db("db.binom", vobj{
//                   {"some_messages", varr{
//                      "first_message",
//                      "second_message",
//                      "third_message"
//                    }}
//                 });

//TcpServer server(8080, [](TcpServer::Client client) {
//  std::string client_addr_str = getHostStr(client);
//  std::clog << "Server:\n|Client " << client_addr_str << " connected\n|Waiting data...\n";
//  while (server.getStatus() == TcpServer::status::up && client.getStatus() == TcpSocketStatus::connected)
//    try {

//      ByteArray client_data;
//      client >> client_data;

//      if(client.getStatus() != TcpSocketStatus::connected) {
//        std::clog << "Client is disconnected!";
//        return;
//      }

//      std::clog << "Server:\n|Recived data from: " << client_addr_str << '\n'
//                << "|Size: " << client_data.length() << " bytes\n";

//      client << db.getRoot()(Path::fromByteArray(client_data)).getVariable().serialize();

//    } catch (Exception& except) {
//      std::string err_msg = except.full();
//      std::clog << "Server:\n|Client " << client_addr_str << " error:\n" << '|' << err_msg << '\n';
//      ByteArray result_data = Variable(vobj{
//                                         {"err", err_msg}
//                                       }).serialize();
//      client.sendData(reinterpret_cast<char*>(result_data.begin()), result_data.length());
//    }
//});

//void testServer() {
//  //Start server
//  if(server.start() == TcpServer::status::up) {
//    std::clog<<"Server:\n|Server is up!"<<std::endl;
////    server.joinLoop(); //Joing to the client handling loop
//    return;
//  } else {
//    std::clog<<"Server:\n|Server start error! Error code:"<< int(server.getStatus()) <<std::endl;
//    std::exit(-1);
//  }
//}


//void testClient() {
//  TcpClient client;
//  if(client.connectTo(LOCALHOST_IP, 8080) == TcpClient::status::connected) {

//    std::thread distant_msg([&client](){ // Server data reciver
//      while (server.getStatus() == TcpServer::status::up) {
//        ByteArray server_data;
//        client >> server_data;

//        if(client.getStatus() != TcpSocketStatus::connected) {
//          std::clog << "Client is disconnected!";
//          return;
//        }

//        Variable data = Variable::deserialize(server_data);
//        if(data.contains("err")) { // Error handling
//          std::clog << "Client:\n|" << data.getVariable("err").toBufferArray().toString() << std::endl;
//        } else std::clog << "Client:\n|Recived data from server: " << data << '\n';
//      }
//    });

//    while(1) try {
//      std::string path;
//      std::clog << "Enter path (exit - for exit from programm): ";
//      std::cin >> path;
//      if(path == "exit") break;
//      client << Path::fromString(path).toByteArray();
//    } catch(const Exception& except) {
//      std::cerr << except.full() << '\n';
//    }

//    server.stop();
//    distant_msg.join();

//  } else {
//    std::clog<<"Client:\n|Error! Client isn't connected! Error code: " << int(client.getStatus ()) << std::endl;
//    server.stop();
//    std::exit(-1);
//  }
//}


//void test() {
//  testServer();
//  testClient();
//}

#endif // BINOM_TCP_IO_H
