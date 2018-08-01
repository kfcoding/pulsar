#include "broker.h"
#include <iostream>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/composite.h>
#include <xcb/damage.h>
#include <xcb/xtest.h>
#include <string.h>
#include "event.h"
#include "../encoder.h"

namespace pulsar {
  namespace graphics {

    Broker::Broker(BackendType backend_type) {
      backend_type_ = backend_type;
    }

    void Broker::Run() {
      std::cout << "Run()" << std::endl;
      if (backend_type_ == kXserver) {
        RunX();
      } else {
        RunW();
      }
    }

    // Xserver broker
    void Broker::RunX() {
      xcb_connection_t *xconn = xcb_connect(NULL, NULL);
      if (xcb_connection_has_error(xconn)) {
        std::cout << "faild to connect xserver" << std::endl;
        return;
      }
      std::cout << "success to connect xserver" << std::endl;
      xconn_ = xconn;

      xcb_screen_t *screen = xcb_setup_roots_iterator(
              xcb_get_setup(xconn)).data;
      xcb_window_t root = screen->root;
      root_ = root;

      const xcb_query_extension_reply_t *query_damage_reply = xcb_get_extension_data(xconn, &xcb_damage_id);
      xcb_damage_query_version_reply(xconn, xcb_damage_query_version(xconn, 1, 1), 0);

      uint32_t mask[] = {XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY};
      xcb_change_window_attributes(xconn, root, XCB_CW_EVENT_MASK, mask);
      xcb_composite_redirect_subwindows(xconn, root, XCB_COMPOSITE_REDIRECT_AUTOMATIC);

      xcb_generic_event_t *evt;
      while ((evt = xcb_wait_for_event(xconn))) {
        Xevent_t xevt;
        switch (evt->response_type & ~0x80) {
          case XCB_CREATE_NOTIFY: {
            xcb_create_notify_event_t *notify = (xcb_create_notify_event_t *) evt;
            xevt.type = kWindowCreate;
            xevt.window_create.wid = notify->window;
            xevt.window_create.x = notify->x;
            xevt.window_create.y = notify->y;
            xevt.window_create.width = notify->width;
            xevt.window_create.height = notify->height;
            xevt.window_create.override = notify->override_redirect;
            break;
          }
          case XCB_DESTROY_NOTIFY: {
            xcb_destroy_notify_event_t *notify = (xcb_destroy_notify_event_t *) evt;
            xevt.type = kWindowDestroy;
            xevt.window_destroy.wid = notify->window;
            break;
          }
          case XCB_MAP_NOTIFY: {
            xcb_map_notify_event_t *n = (xcb_map_notify_event_t *) evt;
            xevt.type = kWindowShow;
            xevt.window_show.wid = n->window;

            xcb_damage_damage_t damage = xcb_generate_id(xconn);
            xcb_damage_create(xconn, damage, n->window, XCB_DAMAGE_REPORT_LEVEL_RAW_RECTANGLES);
            xcb_flush(xconn);

            break;
          }
          case XCB_UNMAP_NOTIFY: {
            xcb_unmap_notify_event_t *n = (xcb_unmap_notify_event_t *) evt;
            xevt.type = kWindowHide;
            xevt.window_hide.wid = n->window;
            break;
          }
          case XCB_CONFIGURE_NOTIFY: {
            xcb_configure_notify_event_t *n = (xcb_configure_notify_event_t *) evt;
            xevt.type = kWindowConfigure;
            xevt.window_configure.wid = n->window;
            xevt.window_configure.x = n->x;
            xevt.window_configure.y = n->y;
            xevt.window_configure.width = n->width;
            xevt.window_configure.height = n->height;
            xevt.window_configure.override = n->override_redirect;
            break;
          }
          default:
            xevt.type = kUnknown;
            if (evt->response_type == query_damage_reply->first_event + XCB_DAMAGE_NOTIFY) {
              xcb_damage_notify_event_t *d = (xcb_damage_notify_event_t *) evt;
              xcb_damage_subtract(xconn, d->damage, XCB_NONE, XCB_NONE);

              encoded_data_t ed;
              ed.size = 0;
              ed.data = NULL;
              GetDamageImage(xconn, d->drawable, d->area.x, d->area.y, d->area.width, d->area.height, &ed);
              if (ed.size > 0) {
                xevt.type = kWindowImage;
                xevt.window_image.wid = d->drawable;
                xevt.window_image.x = d->area.x;
                xevt.window_image.y = d->area.y;
                xevt.window_image.width = d->area.width;
                xevt.window_image.height = d->area.height;
                xevt.window_image.data = ed.data;
                xevt.window_image.size = ed.size;
              } else {
                continue;
              }
            }
            break;
        }
        OnGraphicsEvent(&xevt);
      }
    }

    // Windows broker
    void Broker::RunW() {
      //TODO: Windows implement
    }

    void Broker::GetDamageImage(xcb_connection_t *xconn_, xcb_window_t wid, int16_t x, int16_t y, uint16_t width,
                                uint16_t height, encoded_data_t *ed) {
      xcb_get_image_reply_t *reply = xcb_get_image_reply(xconn_,
                                                         xcb_get_image(xconn_, XCB_IMAGE_FORMAT_Z_PIXMAP, wid, x,
                                                                       y, width, height, ~0), NULL);

      if (reply == NULL) {
        return;
      }

      uint8_t *data = xcb_get_image_data(reply);

      int len = xcb_get_image_data_length(reply);
      for (uint32_t i = 0; i < len; i += 4) {
        uint8_t t = data[i];
        data[i] = data[i + 2];
        data[i + 2] = t;
        //data[i+3] = 0xff;
      }
      Encoder::Encode(width, height, data, ed);
      free(reply);
    }

    void Broker::Input(Cevent_t *cevt) {
      xcb_connection_t *xconn = xconn_;
      switch (cevt->type) {
        case kMouseMove: {
          xcb_warp_pointer(xconn, XCB_NONE, root_, 0, 0, 0, 0, cevt->x, cevt->y);
          break;
        }
        case kMouseDown: {
          xcb_test_fake_input(xconn, XCB_BUTTON_PRESS, cevt->mousedown.code, XCB_CURRENT_TIME, root_, 0, 0, 0);
          break;
        }
        case kMouseUp: {
          xcb_test_fake_input(xconn, XCB_BUTTON_RELEASE, cevt->mousedown.code, XCB_CURRENT_TIME, root_, 0, 0, 0);
          break;
        }
        case kKeyDown: {
          xcb_test_fake_input(xconn, XCB_KEY_PRESS, cevt->keydown.code, XCB_CURRENT_TIME, root_, 0, 0, 0);
          break;
        }
        case kKeyUp: {
          xcb_test_fake_input(xconn, XCB_KEY_RELEASE, cevt->keyup.code, XCB_CURRENT_TIME, root_, 0, 0, 0);
          break;
        }
        case kRecover: {
          std::cout << "this ---> " << std::endl;
          xcb_query_tree_reply_t *tree = xcb_query_tree_reply(xconn, xcb_query_tree(xconn, root_), NULL);
          xcb_window_t *children = xcb_query_tree_children(tree);
          Xevent_t xevt;
          for (int i = 0; i < xcb_query_tree_children_length(tree); ++i) {
            printf("child: %d\n", children[i]);
            xcb_get_geometry_reply_t *geo = xcb_get_geometry_reply(xconn, xcb_get_geometry(xconn, children[i]), NULL);
            xcb_get_window_attributes_reply_t *attr = xcb_get_window_attributes_reply(xconn,
                                                                                      xcb_get_window_attributes(xconn,
                                                                                                                children[i]),
                                                                                      NULL);
            xevt.type = kWindowCreate;
            xevt.window_create.wid = children[i];
            xevt.window_create.x = geo->x;
            xevt.window_create.y = geo->y;
            xevt.window_create.width = geo->width;
            xevt.window_create.height = geo->height;
            xevt.window_create.override = attr->override_redirect;
            OnGraphicsEvent(&xevt);

            if (attr->map_state == XCB_MAP_STATE_VIEWABLE) {
              xevt.type = kWindowShow;
              xevt.window_show.wid = children[i];
              OnGraphicsEvent(&xevt);
            }
          }

          free(tree);
          break;
        }
      }
      xcb_flush(xconn);
    }

  }

}
