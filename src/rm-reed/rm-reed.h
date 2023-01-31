
#ifndef RM_REED_H_
#define RM_REED_H_

#include <stdio.h>
#include <stdbool.h>

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
#include "sys/log.h"

// Timers
#include "sys/etimer.h"
#include "sys/stimer.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_COAP

////////////////////////////////////////////////////////////
// RESOURCES
////////////////////////////////////////////////////////////

static void res_reedStatus_get_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    );

static void res_periodic_handler(void);

PERIODIC_RESOURCE(
    res_reedStatus,
    "title=\"reed/status\";rt=\"JSON\"",
    res_reedStatus_get_handler,
    NULL, // POST 
    NULL, // PUT  
    NULL, // DELETE 
    3000,
    res_periodic_handler
    );

////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// DEBUG
////////////////////////////////////////////////////////////

static struct etimer dbg_timer;

////////////////////////////////////////////////////////////

static struct stimer reed_timer;

static bool    reed_status = true;
static uint8_t reed_message_id = 0;

PROCESS(rm_reed, "Zolertia RE-Mote - Reed Switch");

AUTOSTART_PROCESSES(
    &rm_reed
    );

#endif//RM_REED_H_
