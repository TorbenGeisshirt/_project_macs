
#ifndef CLI_REED_H_
#define CLI_REED_H_

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

#ifdef _WITH_DTLS
    #define REMOTE_REED_EP "coaps://[fe80::212:4b00:abcd:ef02]"
#else
    #define REMOTE_REED_EP "coap://[fe80::212:4b00:abcd:ef02]"
#endif

#define NUMBER_OF_URLS 2

#if LOG_CONF_LEVEL_COAP == LOG_LEVEL_DBG
    #define REQUEST_INTERVAL 10
#else
    #define REQUEST_INTERVAL 5
#endif

////////////////////////////////////////////////////////////
// OBSERVE
////////////////////////////////////////////////////////////

#define OBS_RESOURCE_URI "reed/status"

static coap_observee_t *observee;

static void observe_callback(
    coap_observee_t *obs, 
    void *notification,
    coap_notification_flag_t flag
    );

////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// DEBUG
////////////////////////////////////////////////////////////

static struct etimer dbg_timer;

////////////////////////////////////////////////////////////

char *remote_urls[NUMBER_OF_URLS] =
{
    ".well-known/core",
    
    "reed/status",
};

void client_chunk_handler(coap_message_t *response);

PROCESS(cli_reed, "Zolertia RE-Mote - Client: Reed Switch");

AUTOSTART_PROCESSES(
    &cli_reed
    );

#endif//CLI_REED_H_
