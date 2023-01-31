
#ifndef RM_DLOCK_H_
#define RM_DLOCK_H_

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
#define LOG_LEVEL LOG_LEVEL_APP

////////////////////////////////////////////////////////////
// RESOURCES
////////////////////////////////////////////////////////////

static void res_lockOpen_get_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    );

RESOURCE(
    res_lockOpen,
    "title=\"lock/open\";rt=\"JSON\"",
    res_lockOpen_get_handler,
    NULL, // POST 
    NULL, // PUT  
    NULL  // DELETE 
    );

////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// DEBUG
////////////////////////////////////////////////////////////

static struct etimer dbg_timer;

////////////////////////////////////////////////////////////

static struct stimer lock_timer;

static bool    lock_enabled = false;
static uint8_t lock_message_id = 0;

PROCESS(rm_dlock, "Zolertia RE-Mote - Door Lock");

AUTOSTART_PROCESSES(
    &rm_dlock
    );

#endif//RM_DLOCK_H_
