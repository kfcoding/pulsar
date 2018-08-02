#!/bin/bash
mkdir -p build
gcc main.c ws_server.c pulsar.c xthread.c encoder.c -o build/pulsar -lpthread -lwebp -lpng -lwebsockets -lX11 -lXdamage -lXtst -g
#cp pulsar /usr/bin
