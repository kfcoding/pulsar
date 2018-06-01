FROM daocloud.io/shaoling/kfcoding-rstudio-pulsar:master

COPY pulsar /usr/local/bin/pulsar

ENV DISPLAY :0
ENV PULSAR_PORT 9800
CMD pulsar
EXPOSE 9800
