#ifndef PULSAR_H
#define PULSAR_H
#include <X11/X.h>
#include <X11/extensions/Xdamage.h>
#include <sys/queue.h>
#include <stdint.h>
#include <pthread.h>

/**
 * A rect region
 **/
typedef struct {
    int x;
    int y;
    int width;
    int height;
    int init;
} pulsar_region_t;

typedef struct {
    /**
     * the type of graphics encoding mode
     **/
    enum {
        IMAGE,
        STREAM
    } graphics_mode;
    int port;

    /**
     * pedding list for sending
     **/

} pulsar_config_t;

struct encoded_entry {
    int x;
    int y;
    int size;
    uint8_t *data;
    STAILQ_ENTRY(encoded_entry) pointers;
};

typedef struct {
    pulsar_region_t region; // rect region damaged
    Display *display;
    Window root;
    pthread_t xthread; // X11 thread
    pthread_t ethread; // encoder thread
    pthread_mutex_t mutex;
    STAILQ_HEAD(encoded_list, encoded_entry) pending_list;
} pulsar_context_t;

typedef struct {
    int size;
    uint8_t *data;
} encoded_data_t;

void pulsar_context_init(pulsar_context_t *pulsar_context);
void pulsar_context_region_add(pulsar_context_t *context, int x, int y, int width, int height);

#endif
