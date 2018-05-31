#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "server.h"
#include "graphics/broker.h"

int main(int argc, char *argv[]) {
  int port = 9800;
  
  pulsar::graphics::Broker broker(pulsar::graphics::kXserver);
  pulsar::Server server;

  broker.OnGraphicsEvent = boost::bind(&pulsar::Server::Broadcast, boost::ref(server), _1);

  server.OnClientEvent = boost::bind(&pulsar::graphics::Broker::Input, boost::ref(broker), _1);

  // Start websocket server as thread
  boost::thread trd(boost::bind(&pulsar::Server::Run, boost::ref(server), port));

  // Broker blocks main thread
  broker.Run();
  
  return 0;
}
