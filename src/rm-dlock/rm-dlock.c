
#include "rm-dlock.h"

#include "contiki.h"
#include "contiki-net.h"

#include "dev/gpio.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define GPIO_A_NUM          0
#define LOCK_PIN_MASK       GPIO_PIN_MASK(5)
#define GPIO_A_PORT_BASE    GPIO_PORT_TO_BASE(GPIO_A_NUM)

////////////////////////////////////////////////////////////
// RESOURCES
////////////////////////////////////////////////////////////

static void res_lockOpen_get_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    )
{
    lock_enabled = true;
    stimer_set(&lock_timer, 5);

    char message[COAP_MAX_CHUNK_SIZE] = "";

    char format[] = "{\"message_id\": %d, \"path\": %s, \"lock\": %d}";

    int result = snprintf(
        message, (COAP_MAX_CHUNK_SIZE - 1), format, lock_message_id++, request->uri_path, lock_enabled
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

PROCESS_THREAD(rm_dlock, ev, data)
{
    PROCESS_BEGIN();

#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
    //lwm2m_security_init();
#endif

    PROCESS_PAUSE();

    printf("Activating resources...\n");

    // Activate CoAP resources
    coap_activate_resource(&res_lockOpen, "lock/open");

    // Initialize GPIO states
    GPIO_SOFTWARE_CONTROL(GPIO_A_PORT_BASE, LOCK_PIN_MASK);
    GPIO_SET_OUTPUT(GPIO_A_PORT_BASE, LOCK_PIN_MASK);
    GPIO_CLR_PIN(GPIO_A_PORT_BASE, LOCK_PIN_MASK);
    
    // Initialize debug timer
    etimer_set(
        &dbg_timer, 3 * CLOCK_SECOND
        );

    while(true)
    {
        PROCESS_WAIT_EVENT();

        if (lock_enabled)
            GPIO_SET_PIN(GPIO_A_PORT_BASE, LOCK_PIN_MASK);
        else
            GPIO_CLR_PIN(GPIO_A_PORT_BASE, LOCK_PIN_MASK);

        if (stimer_expired(&lock_timer))
            lock_enabled = false;

        // DBG status information 
        // in fixed time interval
        if (etimer_expired(&dbg_timer))
        {
            uint8_t lock_pin = GPIO_READ_PIN(GPIO_A_PORT_BASE, LOCK_PIN_MASK);
                printf("Lock-Pin: %d\n", lock_pin);

            printf("Lock-Enabled: %d\n", lock_enabled);

            // Reset timer
            etimer_reset(&dbg_timer);
        }
    }

    PROCESS_END();
}
