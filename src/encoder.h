#ifndef PULSAR_ENCODER_H_
#define PULSAR_ENCODER_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace pulsar {

    typedef struct encoded_data {
        int size;
        uint8_t *data;
    } encoded_data_t;

    class Encoder {
    public:
        static void Encode(size_t w, size_t h, uint8_t *data, void *out);
    };

}

#endif
