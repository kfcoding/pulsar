#ifndef PULSAR_GRAPHICS_BROKER_H_
#define PULSAR_GRAPHICS_BROKER_H_

#include <string>
#include <boost/function.hpp>
#include <xcb/xcb.h>
#include "../encoder.h"
#include "event.h"

namespace pulsar {
  namespace graphics {

    enum BackendType {
      kXserver,
      kWindows
    };

    class Broker {
    public:
      Broker(BackendType backend_type);

      void Run();

      void Input(Cevent_t *);

      void set_display(std::string display) { display_ = display; }

      typedef boost::function<void(Xevent_t *)> func_t;
      func_t OnGraphicsEvent;

    private:
      void RunX();

      void RunW();

      static void
      GetDamageImage(xcb_connection_t *xconn, xcb_window_t wid, int16_t x, int16_t y, uint16_t width, uint16_t height,
                     encoded_data_t *ed);

      BackendType backend_type_;
      std::string display_;
      xcb_connection_t *xconn_;
      xcb_window_t root_;
    };

  }
}

#endif
