
#ifndef RES_LOCK_H_
#define RES_LOCK_H_

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* CoAP engine */
#include "coap-engine.h"

//const char* attributes = "title=\"Lock\";rt=\"JSON\"";

static bool lck_state = false;
static int  lck_msg_counter = 0;

static void res_get_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    );

static void res_post_put_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    );

RESOURCE(
    res_lock,
    "title=\"Lock\";rt=\"JSON\"",
    res_get_handler,
    res_post_put_handler,
    res_post_put_handler,
    NULL
    );

#endif//RES_LOCK_H_
