FROM cloudwarelabs/xfce4-min
MAINTAINER wsl <wsl@kfcoding.com>

RUN apt-get update
RUN apt-get install -y libwebp-dev libx11-dev libxdamage-dev libxtst-dev libpng12-0

COPY build/pulsar /usr/local/bin/pulsar
COPY libwebsockets.so.11 /usr/lib/
COPY pulsar.desktop /root/.config/autostart/

ENV PULSAR_PORT 5678

EXPOSE 5678
CMD pulsar
