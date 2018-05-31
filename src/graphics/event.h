#ifndef PULSAR_GRAPHICS_EVENT_H_
#define PULSAR_GRAPHICS_EVENT_H_

namespace pulsar {

  enum EventType {
    kUnknown,
    kWindowCreate,  //1
    kWindowDestroy, //2
    kWindowShow,    //3
    kWindowHide,    //4
    kWindowConfigure,
    kWindowImage,
    kMouseMove,
    kMouseDown,
    kMouseUp, // 10
    kKeyDown,
    kKeyUp,
    kRecover
  };

  typedef struct Cevent {
    uint8_t type;
    uint16_t x;
    uint16_t y;
    union {
      struct {
        uint16_t x;
        uint16_t y;
      } mousemove;

      struct {
        uint32_t code;
      } mousedown;

      struct {
        uint32_t code;
      } mouseup;

      struct {
        uint32_t code;
      } keydown;

      struct {
        uint32_t code;
      } keyup;

      struct {
        uint32_t wid;
      } recover;
    };
  }__attribute__((packed)) Cevent_t;

  typedef struct Xevent {
    EventType type;
    union {
      struct {
        int wid;
        int16_t x;
        int16_t y;
        uint16_t width;
        uint16_t height;
        uint8_t override;
      } window_create;

      struct {
        int wid;
      } window_destroy;

      struct {
        int wid;
      } window_show;

      struct {
        int wid;
      } window_hide;

      struct {
        int wid;
        int16_t x;
        int16_t y;
        uint16_t width;
        uint16_t height;
        uint8_t override;
      } window_configure;

      struct {
        int wid;
        uint size;
        int16_t x;
        int16_t y;
        uint16_t width;
        uint16_t height;
        void *data;
      } window_image;
    };
  }__attribute__((packed)) Xevent_t;

}

#endif
