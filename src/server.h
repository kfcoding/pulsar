#ifndef PULSAR_SERVER_H_
#define PULSAR_SERVER_H_

#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include "graphics/broker.h"
#include "graphics/event.h"

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;

namespace pulsar {

struct connection_data {
  int session_id;
};

class Server {
 public:
  Server();
  void Run(int port);
  void Broadcast(Xevent_t*);

  typedef boost::function<void (Cevent_t*)> func_t;
  func_t OnClientEvent;

 private:
  void OnClientOpen(connection_hdl hdl);
  void OnClientClose(connection_hdl hdl);
  void OnClientMessage(connection_hdl hdl, server::message_ptr msg);
  
  server wsserver_;
  typedef std::map<connection_hdl, connection_data, std::owner_less<connection_hdl>> con_list;
  con_list connections_;

};
}

#endif
