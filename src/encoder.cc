#include "encoder.h"
#include <png.h>

namespace pulsar {

    void png_write_cb(png_structp png_ptr, png_bytep data, png_size_t size) {
        struct encoded_data *ed = (struct encoded_data *) png_get_io_ptr(png_ptr);
        ed->data = (uint8_t *) realloc((uint8_t * )(ed->data), ed->size + size);
        memcpy(&ed->data[ed->size], data, size);
        ed->size += size;
    }

    void Encoder::Encode(size_t w, size_t h, uint8_t *data, void *out) {

        png_structp png_struct_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        png_infop png_info_ptr = png_create_info_struct(png_struct_ptr);
        png_set_IHDR(png_struct_ptr, png_info_ptr, w, h, 8,
                     PNG_COLOR_TYPE_RGBA,
                     PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_DEFAULT,
                     PNG_FILTER_TYPE_DEFAULT);

        uint8_t *rows[h];
        for (size_t i = 0; i < h; i++) {
            rows[i] = (uint8_t *) data + i * w * 4;
        }

        png_set_rows(png_struct_ptr, png_info_ptr, rows);
        png_set_write_fn(png_struct_ptr, out, png_write_cb, NULL);
        png_write_png(png_struct_ptr, png_info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
        png_destroy_write_struct(&png_struct_ptr, &png_info_ptr);
    }

}
