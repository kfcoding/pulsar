#include <pthread.h>
#include <X11/X.h>
#include <X11/extensions/Xdamage.h>
#include <X11/Xthreads.h>
#include <stdint.h>
#include <stdio.h>
#include "pulsar.h"
//#include "xthread.h"


void xthread(void *data)
{
    printf("xthread\n");
    pulsar_context_t *context = (pulsar_context_t*)data;
    Display *display = XOpenDisplay(NULL);
    Window root = RootWindow(display, DefaultScreen(display));
    int damage_event_base, damage_error_base;
    XDamageQueryExtension(display, &damage_event_base, &damage_error_base);
    Damage damage_handle = XDamageCreate(display, root, XDamageReportRawRectangles);
    XEvent xevt;
    XDamageNotifyEvent *damage_event;
    uint8_t *output;
    int output_size, i;

    while (1) {
        XNextEvent(display, &xevt);
        switch (xevt.type) {
        default:
            if (xevt.type == damage_event_base + XDamageNotify) {

                
                damage_event = (XDamageNotifyEvent*)&xevt;

                int x = damage_event->area.x;
                int y = damage_event->area.y;
                int width = damage_event->area.width;
                int height = damage_event->area.height;
                if (width == 0 || height == 0) {
                    break;
                }

                pulsar_context_region_add(context, x, y, width, height);
            }
            break;
        }
    }
}

void start_xthread(pthread_t *t, pulsar_context_t *context)
{
    pthread_create(t, NULL, (void*)xthread, context);
}
