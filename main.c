#include <stdlib.h>
#include <stdio.h>
#include "pulsar.h"
#include "common.h"
#include "ws_server.h"

int main(int argc, char* argv[])
{
    pulsar_config_t *pulsar_config = (pulsar_config_t*)malloc(sizeof(pulsar_config_t));
    pulsar_config->port = 5678;

    
    start_ws_server(pulsar_config);
    return 0;
}
