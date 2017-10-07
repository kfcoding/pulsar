#include <X11/extensions/XTest.h>
#include <X11/extensions/Xdamage.h>
#include <stdlib.h>
#include <stdio.h>
#include "pulsar.h"
#include "common.h"
#include "ws_server.h"



damage_region_t region;

int main(int argc, char* argv[])
{
    pulsar_config_t *pulsar_config = (pulsar_config_t*)malloc(sizeof(pulsar_config_t));
    pulsar_config->port = 5678;

    
    start_ws_server(pulsar_config);
    return 0;
}
