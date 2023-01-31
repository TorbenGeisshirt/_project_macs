
#ifndef RM_BUZZER_H_
#define RM_BUZZER_H_

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

static void res_buzzSound_get_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    );

RESOURCE(
    res_buzzSound,
    "title=\"buzz/sound\";rt=\"JSON\"",
    res_buzzSound_get_handler,
    NULL, // POST 
    NULL, // PUT  
    NULL  // DELETE 
    );

static void res_buzzAlarm_get_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    );

RESOURCE(
    res_buzzAlarm,
    "title=\"buzz/alarm\";rt=\"JSON\"",
    res_buzzAlarm_get_handler,
    NULL, // POST 
    NULL, // PUT  
    NULL  // DELETE 
    );

static void res_buzzReset_get_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    );

RESOURCE(
    res_buzzReset,
    "title=\"buzz/reset\";rt=\"JSON\"",
    res_buzzReset_get_handler,
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

static struct stimer buzz_timer;

static bool    buzz_alarm = false;
static bool    buzz_enabled = false;
static uint8_t buzz_message_id = 0;

PROCESS(rm_buzzer, "Zolertia RE-Mote - Piezo Buzzer");

AUTOSTART_PROCESSES(
    &rm_buzzer
    );

#endif//RM_BUZZER_H_
