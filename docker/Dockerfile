FROM registry.cn-shanghai.aliyuncs.com/kfcoding/workspace-envs:cloudware-base

COPY libwebsockets.so.14 /usr/lib/

RUN mkdir -p /opt/pulsar/www/html
COPY desktop.html /opt/pulsar/www/html/desktop.html

COPY server /usr/local/bin/server
RUN chmod a+x /usr/local/bin/server
COPY server.desktop /root/.config/autostart/

COPY pulsar /usr/local/bin/pulsar
RUN chmod a+x /usr/local/bin/pulsar
COPY pulsar.desktop /root/.config/autostart/

ENTRYPOINT ["startxfce4"]