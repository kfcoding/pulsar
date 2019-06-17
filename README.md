# pulsar v1.1
http & ws server


## How to build

```
mkdir -p build

gcc main.c ws_server.c pulsar.c xthread.c encoder.c -o build/pulsar -Ilibwebsockets/lib -Ilibwebsockets/build -Llibwebsockets/build/lib -lwebsockets \
    -lpthread -lwebp -lpng -lX11 -lXdamage -lXtst -g
```

## How to run

```
build/pulsar
```