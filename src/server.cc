#include "server.h"
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::connection_hdl;

namespace pulsar {

  Server::Server() {
    wsserver_.init_asio();
    wsserver_.set_access_channels(websocketpp::log::alevel::none);
    wsserver_.set_open_handler(bind(&Server::OnClientOpen, this, ::_1));
    wsserver_.set_close_handler(bind(&Server::OnClientClose, this, ::_1));
    wsserver_.set_message_handler(bind(&Server::OnClientMessage, this, ::_1, ::_2));
  }

  void Server::Run(int port) {
    wsserver_.listen(port);
    wsserver_.start_accept();
    wsserver_.run();
  }

  void Server::OnClientOpen(connection_hdl hdl) {
    connection_data data;
    connections_[hdl] = data;

    system("nohup /usr/src/rstudio-1.0.136/bin/rstudio &");
    //system("/usr/src/rstudio-1.0.136/bin/rstudio");

//    Cevent_t
//    c.type = kRecover;
//    c.recover.wid = 0;
//    OnClientEvent(&c);

    std::cout << "OnClientOpen" << std::endl;
  }

  void Server::OnClientClose(connection_hdl hdl) {
    std::cout << "OnClientClose" << std::endl;
  }

  void Server::OnClientMessage(connection_hdl hdl, server::message_ptr msg) {
    //  std::cout << msg->get_payload() << std::endl;
    const char *evt = msg->get_payload().c_str();
    Cevent_t cevt;
    switch (evt[0]) {
      case kMouseMove: {
        cevt.type = kMouseMove;
        memcpy(&cevt.x, &evt[1], 2);
        memcpy(&cevt.y, &evt[3], 2);
        break;
      }
      case kMouseDown: {
        cevt.type = kMouseDown;
        memcpy(&cevt.mousedown.code, &evt[1], 4);
        break;
      }
      case kMouseUp: {
        cevt.type = kMouseUp;
        memcpy(&cevt.mouseup.code, &evt[1], 4);
        break;
      }
      case kKeyDown: {
        cevt.type = kKeyDown;
        memcpy(&cevt.keydown.code, &evt[1], 4);
        break;
      }
      case kKeyUp: {
        cevt.type = kKeyUp;
        memcpy(&cevt.keyup.code, &evt[1], 4);
        break;
      }
      default:
        return;
    }
    OnClientEvent(&cevt);

  }

  void Server::Broadcast(Xevent_t *xevt) {
    con_list::iterator it;
    it = connections_.begin();
    while (it != connections_.end()) {
      if (xevt->type == kWindowImage) {
        int size = sizeof(Xevent_t) + xevt->window_image.size;
        uint8_t *data = (uint8_t *) malloc(size);
        memcpy(data, xevt, sizeof(Xevent_t));
        memcpy(&data[sizeof(Xevent_t)], xevt->window_image.data, xevt->window_image.size);
        wsserver_.send(it->first, data, size, websocketpp::frame::opcode::binary);
        free(data);
      } else {
        wsserver_.send(it->first, xevt, sizeof(Xevent_t), websocketpp::frame::opcode::binary);
      }
      it++;
    }
  }

}
