#include <png.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <X11/Xthreads.h>
#include "pulsar.h"
#include "encoder.h"

struct encode_context {
    int x;
    int y;
    int width;
    int height;
    pulsar_context_t *pc;
    XImage *ximg;
};

void RGBAtoPNG(int width, int height, uint8_t *dataRGBA, void *user)
{

}

void png_write_cb(png_structp png_ptr, png_bytep data, png_size_t length)
{
    encoded_data_t *ed = (encoded_data_t*)png_get_io_ptr(png_ptr);
    ed->data = realloc(ed->data, ed->size + length);
    memcpy(&ed->data[ed->size], data, length);
    ed->size += length;
}

/**
 * encode RGBA data to PNG
 **/
void encodePNG(size_t w, size_t h, uint8_t *data, void *out)
{
    png_structp png_struct_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop png_info_ptr = png_create_info_struct(png_struct_ptr);
    png_set_IHDR(png_struct_ptr, png_info_ptr, w, h, 8,
                 PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    uint8_t *rows[h];
    int i = 0;
    for (i; i < h; i++) {
        rows[i] = (uint8_t*)data + i * w * 4;
    }
    png_set_rows(png_struct_ptr, png_info_ptr, rows);
    png_set_write_fn(png_struct_ptr, out, png_write_cb, NULL);
    png_write_png(png_struct_ptr, png_info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_destroy_write_struct(&png_struct_ptr, &png_info_ptr);
}

void encoder_thread(void *data)
{
    pulsar_context_t *context = data;
    Display *display = XOpenDisplay(NULL);
    Window root = RootWindow(display, DefaultScreen(display));
    while (1) {
        printf("%d %d %d %d\n", context->region.x, context->region.y, context->region.width, context->region.height);
        if (context->region.width != 0 && context->region.height != 0) {printf("enc\n");
            int x = context->region.x;
            int y = context->region.y;
            int width = context->region.width;
            int height = context->region.height;
            XImage *ximg;
            printf("%d %d %d %d\n", x, y, width, height);
            ximg = XGetImage(display, root, x, y, width, height, AllPlanes, ZPixmap);

            int i;
            for (i = 3; i < (width * height << 2); i += 4) {
                ximg->data[i] = 0xff;
            }

            struct encode_context *ec = malloc(sizeof(struct encode_context)); // freed in encoded callback
            ec->x = x;
            ec->y = y;
            ec->width = width;
            ec->height = height;
            ec->pc = context;
            ec->ximg = ximg;
            encodePNG(width, height, ximg->data, ec);printf("2\n");
            XDestroyImage(ximg);
            context->region.x = 0;
            context->region.y = 0;
            context->region.width = 0;
            context->region.height = 0;
        }
        //usleep(20000);
    }
}

void start_encoder_thread(pthread_t *t, pulsar_context_t *context)
{
    pthread_create(t, NULL, (void*)encoder_thread, context);
}
