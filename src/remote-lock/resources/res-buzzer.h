
#ifndef RES_BUZZER_H_
#define RES_BUZZER_H_

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* CoAP engine */
#include "coap-engine.h"

//const char* attributes = "title=\"Buzzer\";rt=\"JSON\"";

static bool bz_state = false;
static int  bz_msg_counter = 0;

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
    res_buzzer,
    "title=\"Buzzer\";rt=\"JSON\"",
    res_get_handler,
    res_post_put_handler,
    res_post_put_handler,
    NULL
    );

#endif//RES_BUZZER_H_
