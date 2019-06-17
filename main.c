#include <stdlib.h>
#include <stdio.h>
#include "pulsar.h"
#include "common.h"
#include "ws_server.h"

int main(int argc, char* argv[])
{
    char *pulsar_port_env = getenv("PULSAR_PORT");
    int pulsar_port = pulsar_port_env ? atoi(pulsar_port_env) : 5679;
    pulsar_config_t *pulsar_config = (pulsar_config_t*)malloc(sizeof(pulsar_config_t));
    pulsar_config->port = pulsar_port;

    start_ws_server(pulsar_config);
    return 0;
}
