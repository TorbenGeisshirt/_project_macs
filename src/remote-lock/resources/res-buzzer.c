
#include "res-buzzer.h"

static void res_get_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    )
{
    char message[COAP_MAX_CHUNK_SIZE] = "";

    int result = snprintf(
        message, 
        COAP_MAX_CHUNK_SIZE - 1,
        "{\"message_id\": %d, \"buzzer/toggle\": %d}",  
        bz_msg_counter, 
        bz_state
        );
    
    bz_msg_counter++;

    if (result < 0) {
        puts("Error while encoding message");
    } else {
        puts("Sending temperature value");

        int length = strlen(message);
        memcpy(buffer, message, length);
        
        coap_set_header_content_format(response, APPLICATION_JSON);
        coap_set_header_etag(response, (uint8_t*)&length, 1);
        coap_set_payload(response, buffer, length);
    }
}

static void res_post_put_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    )
{
    puts(
        (char *)(request->payload)
        );
}
