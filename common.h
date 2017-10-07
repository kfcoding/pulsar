#ifndef PULSAR_COMMON_H
#define PULSAR_COMMON_H

#define MOUSEMOVE 0
#define MOUSEDOWN 1
#define MOUSEUP 2
#define KEYDOWN 3
#define KEYUP 4

#define ENCODER_PNG 0
#define ENCODER_WEBP 1

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int init;
} damage_region_t;

#endif
