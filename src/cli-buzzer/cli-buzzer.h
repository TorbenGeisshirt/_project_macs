
#ifndef CLI_BUZZER_H_
#define CLI_BUZZER_H_

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
    #define REMOTE_BUZZ_EP "coaps://[fe80::212:4b00:abcd:ef01]"
#else
    #define REMOTE_BUZZ_EP "coap://[fe80::212:4b00:abcd:ef01]"
#endif

#define NUMBER_OF_URLS 4

#if LOG_CONF_LEVEL_COAP == LOG_LEVEL_DBG
    #define REQUEST_INTERVAL 10
#else
    #define REQUEST_INTERVAL 5
#endif

static struct etimer timer;

char *remote_urls[NUMBER_OF_URLS] =
{
    ".well-known/core",
    
    "buzz/sound",
    "buzz/alarm",
    "buzz/reset",
};

void client_chunk_handler(coap_message_t *response);

PROCESS(cli_buzzer, "Zolertia RE-Mote - Client: Piezo Buzzer");

AUTOSTART_PROCESSES(
    &cli_buzzer
    );

#endif//CLI_BUZZER_H_
