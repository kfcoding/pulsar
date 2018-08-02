#!/bin/bash
xfconf-query -c xsettings -p /Net/ThemeName -s "Adwaita"
nohup Xorg :0 -noreset -logfile /tmp/0.log -config /etc/xorg.conf &
sleep 1
nohup xfce4-session &

# monitor pulsar exit, then restart
while :
do
    ps ax | egrep "pulsar$"
    if [ $? -ne 0 ]
    then
        nohup pulsar &
    fi
    sleep 2
done
