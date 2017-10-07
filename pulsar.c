#include <pthread.h>
#include "pulsar.h"

void pulsar_context_init(pulsar_context_t *pulsar_context)
{
    pulsar_context->region.x = 1440;
    pulsar_context->region.y = 900;
    pulsar_context->region.width = 0;
    pulsar_context->region.height = 0;
    pulsar_context->region.init = 1;
    pulsar_context->display = XOpenDisplay(NULL);
    pulsar_context->root = RootWindow(pulsar_context->display, DefaultScreen(pulsar_context->display));
    STAILQ_INIT(&pulsar_context->pending_list);
    pthread_mutex_init(&pulsar_context->mutex, NULL);
}

void pulsar_context_region_add(pulsar_context_t *context, int x, int y, int width, int height)
{
    pulsar_region_t *region = &context->region;
    if (region->init) {
        region->x = x;
        region->y = y;
        region->width = width;
        region->height = height;
        region->init = 0;
        return;
    }
    int x1 = x + width;
    int y1 = y + height;
    int ori_x1 = region->x + region->width;
    int ori_y1 = region->y + region->height;
    if (x < region->x) {
        region->x = x;
    }
    if (y < region->y) {
        region->y = y;
    }
    if (x1 > ori_x1) {
        region->width = x1 - region->x;
    } else {
        region->width = ori_x1 - region->x;
    }

    if (y1 > ori_y1) {
        region->height = y1 - region->y;
    } else {
        region->height = ori_y1 - region->y;
    }
}
