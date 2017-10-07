#ifndef ENCODER_H
#define ENCODER_H

#include <pthread.h>

void start_encoder_thread(pthread_t *t, pulsar_context_t *context);

#endif
