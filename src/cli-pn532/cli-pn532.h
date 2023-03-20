
#ifndef CLI_PN532_H_
#define CLI_PN532_H_

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
/*
#ifdef WITH_DTLS
    #define REMOTE_PN532_EP "coaps://[fe80::212:4b00:abcd:ef09]"
#else
    #define REMOTE_PN532_EP "coap://[fe80::212:4b00:abcd:ef09]"
#endif
*/
#define REMOTE_PN532_EP "coap://[fe80::212:4b00:abcd:ef09]"

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
    
    "pn532/read",
};

void client_chunk_handler(coap_message_t *response);

PROCESS(cli_pn532, "Zolertia RE-Mote - Client: PN532 RFID/NFC");

AUTOSTART_PROCESSES(
    &cli_pn532
    );

#endif//CLI_PN532_H_
