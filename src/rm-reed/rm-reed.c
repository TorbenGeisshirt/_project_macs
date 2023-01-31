
#include "rm-reed.h"

#include "contiki.h"
#include "contiki-net.h"

#include "dev/gpio.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define GPIO_A_NUM          0
#define REED_PIN_MASK       GPIO_PIN_MASK(5)
#define GPIO_A_PORT_BASE    GPIO_PORT_TO_BASE(GPIO_A_NUM)

////////////////////////////////////////////////////////////
// RESOURCES
////////////////////////////////////////////////////////////

static void res_reedStatus_get_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    )
{
    char message[COAP_MAX_CHUNK_SIZE] = "";

    char format[] = "{\"message_id\": %d, \"path\": %s, \"reed\": %d}";

    int result = snprintf(
        message, (COAP_MAX_CHUNK_SIZE - 1), format, reed_message_id++, request->uri_path, reed_status
        );
    
    if (result < 0)
        puts("Error while encoding message!");
    else 
    {
        puts("Sending response!");

        int length = strlen(message);
        memcpy(buffer, message, length);
        
        coap_set_header_content_format(response, APPLICATION_JSON);
        coap_set_header_max_age(response, res_reedStatus.periodic->period / CLOCK_SECOND);
        coap_set_payload(response, buffer, length);
    }
}

static void res_periodic_handler(void)
{
    coap_notify_observers(&res_reedStatus);
}

////////////////////////////////////////////////////////////

PROCESS_THREAD(rm_reed, ev, data)
{
    PROCESS_BEGIN();

#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
    //lwm2m_security_init();
#endif

    PROCESS_PAUSE();

    printf("Activating resources...\n");

    // Activate CoAP resources
    coap_activate_resource(&res_reedStatus, "reed/status");

    // Initialize GPIO states
    GPIO_SOFTWARE_CONTROL(GPIO_A_PORT_BASE, REED_PIN_MASK);
    GPIO_SET_INPUT(GPIO_A_PORT_BASE, REED_PIN_MASK);

    // Initialize debug timer
    etimer_set(
        &dbg_timer, 3 * CLOCK_SECOND
        );

    while(true)
    {
        PROCESS_WAIT_EVENT();

        if (stimer_expired(&reed_timer))
        {
            uint8_t reed_pin = GPIO_READ_PIN(GPIO_A_PORT_BASE, REED_PIN_MASK);
                reed_status = reed_pin 
                    ? true 
                    : false;

            stimer_reset(&reed_timer);
        }

        // DBG status information 
        // in fixed time interval
        if (etimer_expired(&dbg_timer))
        {
            uint8_t reed_pin = GPIO_READ_PIN(GPIO_A_PORT_BASE, REED_PIN_MASK);
                printf("Reed-Pin: %d\n", reed_pin);

            // Reset timer
            etimer_reset(&dbg_timer);
        }
    }

    PROCESS_END();
}
