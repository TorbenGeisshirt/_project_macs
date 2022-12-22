
#ifndef CLIENT_UBT_H_
#define CLIENT_UBT_H_

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

#define NUMBER_OF_URLS 4

#if LOG_CONF_LEVEL_COAP == LOG_LEVEL_DBG
    #define REQUEST_INTERVAL 10
#else
    #define REQUEST_INTERVAL 5
#endif


char *remote_urls[NUMBER_OF_URLS] =
{
    ".well-known/core",
    "reed",
    "lock/open",
    "buzzer/toggle"
};

static struct etimer timer;

void client_chunk_handler(coap_message_t *response);

PROCESS(client_ubt, "Re-Mote: Client UBt");

AUTOSTART_PROCESSES(
    &client_ubt
    );

#endif//CLIENT_UBT_H_
