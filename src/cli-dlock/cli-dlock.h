
#ifndef CLI_DLOCK_H_
#define CLI_DLOCK_H_

#include <string.h>

// Contiki
#include "contiki.h"
#include "contiki-net.h"

//CoAP
#include "coap-engine.h"
#include "coap-blocking-api.h"
#include "coap-callback-api.h"

// LWM
#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
#include "lwm2m-object.h"
#include "lwm2m-engine.h"
#include "lwm2m-security.h"
#endif

// Logging
#include "coap-log.h"

// Timer
#include "sys/etimer.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_COAP

#ifdef WITH_DTLS
    #define REMOTE_LOCK_EP "coaps://[fe80::212:4b00:abcd:ef00]"
#else
    #define REMOTE_LOCK_EP "coap://[fe80::212:4b00:abcd:ef00]"
#endif

#define NUMBER_OF_URLS 2

#if LOG_CONF_LEVEL_COAP == LOG_LEVEL_DBG
    #define REQUEST_INTERVAL 10
#else
    #define REQUEST_INTERVAL 5
#endif

static struct etimer timer;

char *remote_urls[NUMBER_OF_URLS] =
{
    ".well-known/core",
    
    "lock/open",
};

void client_chunk_handler(coap_message_t *response);

PROCESS(cli_dlock, "Zolertia RE-Mote - Client: Door Lock");

AUTOSTART_PROCESSES(
    &cli_dlock
    );

#endif//CLI_DLOCK_H_
