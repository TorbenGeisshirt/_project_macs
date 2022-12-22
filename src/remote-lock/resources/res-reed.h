
#ifndef RES_REED_H_
#define RES_REED_H_

#include <string.h>
#include <stdlib.h>

/* CoAP engine */
#include "coap-engine.h"

//const char* attributes = "title=\"Reed\";rt=\"JSON\"";

static void res_get_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    );

static void res_event_handler(void);

EVENT_RESOURCE(
    res_reed,
    "title=\"Reed\";rt=\"JSON\"",
    res_get_handler,
    NULL,
    NULL,
    NULL,
    res_event_handler
    );

#endif//RES_REED_H_
