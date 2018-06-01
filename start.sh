#!/bin/bash
rm -rf /home/wsl/tmp
docker rm -f xorg
docker rm -f pulsar
docker run --name xorg -p 9800:9800 -v /home/wsl/tmp:/tmp daocloud.io/shaoling/kfcoding-xorg:master-094594c Xorg
# docker run --name pulsar --net=container:xorg -v /home/wsl/tmp:/tmp daocloud.io/shaoling/kfcoding-rstudio-pulsar:master pulsar