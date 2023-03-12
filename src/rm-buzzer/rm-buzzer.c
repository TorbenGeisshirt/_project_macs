
#include "rm-buzzer.h"

#include "contiki.h"
#include "dev/gpio.h"

#define GPIO_A_NUM          0
#define BUZZ_PIN_MASK       GPIO_PIN_MASK(2)
#define GPIO_A_PORT_BASE    GPIO_PORT_TO_BASE(GPIO_A_NUM)

////////////////////////////////////////////////////////////
// RESOURCE - GET HANDLER
////////////////////////////////////////////////////////////

static void res_buzzSound_get_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    )
{
    buzz_enabled = true;
    stimer_set(&buzz_timer, 2);

    char message[COAP_MAX_CHUNK_SIZE] = "";

    char format[] = "{\"message_id\": %d, \"path\": %s, \"alarm\": %d}";

    int result = snprintf(
        message, (COAP_MAX_CHUNK_SIZE - 1), format, buzz_message_id++, request->uri_path, buzz_alarm
        );
    
    if (result < 0)
        puts("Error while encoding message!");
    else 
    {
        puts("Sending response!");

        int length = strlen(message);
        memcpy(buffer, message, length);
        
        coap_set_header_content_format(response, APPLICATION_JSON);
        coap_set_header_etag(response, (uint8_t*)&length, 1);
        coap_set_payload(response, buffer, length);
    }
}

static void res_buzzAlarm_get_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    )
{
    buzz_alarm = true;
    buzz_enabled = true;
    stimer_set(&buzz_timer, 1);

    char message[COAP_MAX_CHUNK_SIZE] = "";

    char format[] = "{\"message_id\": %d, \"path\": %s, \"alarm\": %d}";

    int result = snprintf(
        message, (COAP_MAX_CHUNK_SIZE - 1), format, buzz_message_id++, request->uri_path, buzz_alarm
        );
    
    if (result < 0)
        puts("Error while encoding message!");
    else 
    {
        puts("Sending response!");

        int length = strlen(message);
        memcpy(buffer, message, length);
        
        coap_set_header_content_format(response, APPLICATION_JSON);
        coap_set_header_etag(response, (uint8_t*)&length, 1);
        coap_set_payload(response, buffer, length);
    }
}

static void res_buzzReset_get_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    )
{
    buzz_alarm = false;

    char message[COAP_MAX_CHUNK_SIZE] = "";

    char format[] = "{\"message_id\": %d, \"path\": %s, \"alarm\": %d}";

    int result = snprintf(
        message, (COAP_MAX_CHUNK_SIZE - 1), format, buzz_message_id++, request->uri_path, buzz_alarm
        );
    
    if (result < 0)
        puts("Error while encoding message!");
    else 
    {
        puts("Sending response!");

        int length = strlen(message);
        memcpy(buffer, message, length);
        
        coap_set_header_content_format(response, APPLICATION_JSON);
        coap_set_header_etag(response, (uint8_t*)&length, 1);
        coap_set_payload(response, buffer, length);
    }
}

////////////////////////////////////////////////////////////

PROCESS_THREAD(rm_buzzer, ev, data)
{
    PROCESS_BEGIN();

#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
    //lwm2m_security_init();
#endif

    PROCESS_PAUSE();

    printf("Activating resources...\n");
    
    /* Activate CoAP resources */
    coap_activate_resource(&res_buzzSound, "buzz/sound");
    coap_activate_resource(&res_buzzAlarm, "buzz/alarm");

    GPIO_SOFTWARE_CONTROL(GPIO_A_PORT_BASE, BUZZ_PIN_MASK);
    GPIO_SET_OUTPUT(GPIO_A_PORT_BASE, BUZZ_PIN_MASK);
    GPIO_SET_PIN(GPIO_A_PORT_BASE, BUZZ_PIN_MASK);
    
    // Initialize debug timer
    etimer_set(
        &dbg_timer, 3 * CLOCK_SECOND
        );

    while (true) 
    {
        PROCESS_WAIT_EVENT();

        if (buzz_enabled)
            GPIO_CLR_PIN(GPIO_A_PORT_BASE, BUZZ_PIN_MASK);
        else
            GPIO_SET_PIN(GPIO_A_PORT_BASE, BUZZ_PIN_MASK);

        if (stimer_expired(&buzz_timer))
        {
            /*
            if (!buzz_alarm)
                buzz_enabled = false;
            else
            {
            */
                buzz_enabled = false;
            /*
                // Reset buzz timer
                stimer_reset(
                    &buzz_timer
                    );
            }
            */
        }

        // DBG status information 
        // in fixed time interval
        if (etimer_expired(&dbg_timer))
        {
            uint8_t buzz_pin = GPIO_READ_PIN(GPIO_A_PORT_BASE, BUZZ_PIN_MASK);
                printf("Buzz-Pin: %d\n", buzz_pin);

            // Reset timer
            etimer_reset(&dbg_timer);
        }
    }

    PROCESS_END();
}
