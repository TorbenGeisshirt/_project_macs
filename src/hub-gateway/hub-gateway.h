
#ifndef HUB_GATEWAY_H_
#define HUB_GATEWAY_H_

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
    #define REMOTE_LOCK_EP "coaps://[fe80::212:4b00:abcd:ef05]"
    #define REMOTE_BUZZ_EP "coaps://[fe80::212:4b00:abcd:ef07]"
  //#define REMOTE_REED_EP "coaps://[fe80::212:4b00:abcd:ef02]"
#else
    #define REMOTE_LOCK_EP "coap://[fe80::212:4b00:abcd:ef05]"
    #define REMOTE_BUZZ_EP "coap://[fe80::212:4b00:abcd:ef07]"
#endif

#define REMOTE_REED_EP "coap://[fe80::212:4b00:abcd:ef02]"

#define NUMBER_OF_URLS 6

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

static struct etimer timer;

char *remote_urls[NUMBER_OF_URLS] =
{
    ".well-known/core",
    
    "lock/open",
    "buzz/sound",
    "buzz/alarm",
    "buzz/reset",
    "reed/status",
};

void client_chunk_handler(coap_message_t *response);

PROCESS(hub_gateway, "Zolertia RE-Mote - Hub Gateway");

AUTOSTART_PROCESSES(
    &hub_gateway
    );

#endif//HUB_GATEWAY_H_
