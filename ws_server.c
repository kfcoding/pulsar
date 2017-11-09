#include <libwebsockets.h>
#include <stdio.h>
#include <X11/extensions/Xdamage.h>
#include "ws_server.h"
#include "common.h"
#include "pulsar.h"
#include "encoder.h"

extern damage_region_t region;
Display *display;
Window root;

static int callback_protocol(struct lws *wsi,
                             enum lws_callback_reasons reason,
                             void *user,
                             void *in,
                             size_t len)
{
    
    switch (reason) {
    case LWS_CALLBACK_ESTABLISHED: {
        pulsar_context_t *pc = user;
        Display *display = pc->display;
        Window root = pc->root;
        printf("connection established\n");
        pulsar_context_init(pc);
        start_xthread(&pc->xthread, pc);
        usleep(20000);
        //start_encoder_thread(&pc->ethread, pc);
        pulsar_context_region_add(pc, 0, 0, 1440, 900);
        lws_callback_on_writable(wsi);
        break;
    }
    case LWS_CALLBACK_RECEIVE: {
        pulsar_context_t *pc = user;
        Display *display = pc->display;
        Window root = pc->root;
        char *data = (char*)in;
        uint8_t type;
        memcpy(&type, in, 1);
        switch (type) {
        case MOUSEMOVE: {
            uint16_t x, y;
            memcpy(&x, in + 1, 2);
            memcpy(&y, in + 3, 2);
            XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
            XFlush(display);
            break;
        }
        case MOUSEDOWN: {
            uint32_t code = 0;
            memcpy(&code, in + 1, 4);
            XTestFakeButtonEvent(display, code, True, CurrentTime);
            XFlush(display);
            break;
        }
        case MOUSEUP: {
            uint32_t code = 0;
            memcpy(&code, in + 1, 4);
            XTestFakeButtonEvent(display, code, False, CurrentTime);
            XFlush(display);
            break;
        }
        case KEYDOWN: {
            uint32_t code = 0;
            memcpy(&code, in + 1, 4);
            XTestFakeKeyEvent(display, code, True, CurrentTime);
            XFlush(display);
            break;
        }
        case KEYUP: {
            uint32_t code = 0;
            memcpy(&code, in + 1, 4);
            XTestFakeKeyEvent(display, code, False, CurrentTime);
            XFlush(display);
            break;
        }
        default:
            break;
        }

        break;
    }
    case LWS_CALLBACK_SERVER_WRITEABLE: {
        pulsar_context_t *pc = user;
        Display *display = pc->display;

        Window root = pc->root;
        pulsar_region_t *region = &pc->region;

        /**
         * send ping msg
         **/
        if (difftime(time(NULL), pc->lasttime) > 10) {
            uint8_t *pingbuf = calloc(LWS_PRE + 1, sizeof(char));
            lws_write(wsi, &pingbuf[LWS_PRE], 1, LWS_WRITE_TEXT);
            pc->lasttime = time(NULL);
        }

        int output_size, i;
        uint8_t *output;
        int x, y, width, height;
        if (!region->init) {
            width = region->width - (region->width % 4);
            height = region->height - (region->height % 4);
            
            if (width == 0 || height == 0) {
                break;
            }
            x = region->x;
            y = region->y;
            
            region->x = 0;
            region->y = 0;
            region->width = 0;
            region->height = 0;
            region->init = 1;
            //printf("%d %d %d %d\n", x, y, width, height);
            XImage *ximg;
            ximg = XGetImage(display, root, x, y, width, height, AllPlanes, ZPixmap);

            for (i = 3; i < (width * height << 2); i += 4) {
                ximg->data[i] = ximg->data[i - 3];
                ximg->data[i - 3] = ximg->data[i - 1];
                ximg->data[i - 1] = ximg->data[i];
                ximg->data[i] = 0xff;
            }
            encoded_data_t *ed = malloc(sizeof(encoded_data_t));
            ed->size = 0;
            ed->data = NULL;
            encodePNG(width, height, ximg->data, ed);
            output_size = ed->size;
            output = ed->data;
            //output_size = WebPEncodeBGRA(ximg->data, width, height, width << 2, 80, &output);
            unsigned char *buf = malloc(LWS_PRE + output_size + 8);
            memset(buf, 0, LWS_PRE + output_size + 8);
            memcpy(buf + LWS_PRE, &x, 4);
            memcpy(buf + LWS_PRE + 4, &y, 4);
            memcpy(buf + LWS_PRE + 8, output, output_size);
            lws_write(wsi, &buf[LWS_PRE], output_size + 8, LWS_WRITE_BINARY);
            free(buf);
            free(ed->data);
            free(ed);
            XDestroyImage(ximg);
            
        }
        lws_callback_on_writable(wsi);
        usleep(20000);
        break;
    }
    default:
        break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "cloudware-interacting-protocol",
        callback_protocol,
        sizeof(pulsar_context_t),
        8000000
    },
    {
        NULL, NULL, 0
    }
};

void start_ws_server(pulsar_config_t *pulsar_config)
{
    display = XOpenDisplay(NULL);
    root = RootWindow(display, DefaultScreen(display));
    struct lws_context *context;
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    info.port = pulsar_config->port;
    info.protocols = protocols;
    info.ka_time = 30;
    info.ka_probes = 1;
    info.ka_interval = 1;

    context = lws_create_context(&info);

    while (1) {
        //lws_callback_on_writable_all_protocol(context, protocols);
        lws_service(context, 20);
        usleep(10000);
    }
}
