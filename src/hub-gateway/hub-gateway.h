
#ifndef HUB_GATEWAY_H_
#define HUB_GATEWAY_H_

#include <string.h>

// Contiki
#include "contiki.h"
#include "coap-engine.h"

// LWM
#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
#include "lwm2m-object.h"
#include "lwm2m-engine.h"
#include "lwm2m-security.h"
#endif

// Logging
#include "coap-log.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_DBG

#ifdef WITH_DTLS
    #define REMOTE_EP "coaps://[fe80::212:4b00:abcd:ef00]"
#else
    #define REMOTE_EP "coap://[fe80::212:4b00:abcd:ef00]"
#endif

#define NUMBER_OF_URLS 5

#if LOG_CONF_LEVEL_COAP == LOG_LEVEL_DBG
    #define REQUEST_INTERVAL 10
#else
    #define REQUEST_INTERVAL 5
#endif


char *remote_urls[NUMBER_OF_URLS] =
{
    ".well-known/core",
    
    "lock/open",
    "buzz/sound",
    "buzz/status",
    "reed/status",
};

static struct etimer timer;

void client_chunk_handler(coap_message_t *response);

PROCESS(hub_gateway, "Zolertia RE-Mote - Hub Gateway");

AUTOSTART_PROCESSES(
    &hub_gateway
    );

#endif//HUB_GATEWAY_H_
