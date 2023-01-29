
#include "hub-gateway.h"

// Contiki
#include "contiki-net.h"

// Coap
#include "coap-blocking-api.h"
#include "coap-callback-api.h"

#include "dev/button-hal.h"

void client_chunk_handler(coap_message_t *response)
{
    if(response == NULL) 
    {
        puts(
            "Timeout received!"
            );
        return;
    }

    const uint8_t *chunk;
    const int      length = coap_get_payload(response, &chunk);

    printf(
        "|%.*s", length, (char *)chunk
    );
}

PROCESS_THREAD(hub_gateway, ev, data)
{
    //button_hal_button_t *user_btn;
    static coap_endpoint_t remote_ep;

#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
    //lwm2m_security_init();
#endif

    PROCESS_BEGIN();

    static coap_message_t request_lock[1];
    static coap_message_t request_buzz[1];
    static coap_message_t request_reed[1];

    puts(
        "Zolertia RE-Mote: Hub Gateway initiated!"
        );

    // Endpoint configuration
    coap_endpoint_parse(
        REMOTE_EP, strlen(REMOTE_EP), &remote_ep
        );

    puts(
        "Server address:"
        );
    
    LOG_INFO_COAP_EP(&remote_ep);
    LOG_INFO_("\n");

    if (coap_endpoint_is_secure(&remote_ep))
        puts("Confidentiality is protected by DTLS");
    else
        puts("Confidentiality is not protected - DTLS is not enabled!");
    
    // Connect to remote endpoint
    coap_endpoint_connect(&remote_ep);

    // Initialize timer
    etimer_set(
        &timer, REQUEST_INTERVAL * CLOCK_SECOND
        );

    while (true)
    {
        PROCESS_YIELD();

        // Get status information 
        // in fixed time interval
        if (etimer_expired(&timer))
        {
            // Request GET buzz/status
            coap_init_message(
                request_buzz, COAP_TYPE_CON, COAP_GET, 0
                );
            coap_set_header_uri_path(
                request_buzz, remote_urls[3]
                );

            COAP_BLOCKING_REQUEST(
                &remote_ep, request_buzz, client_chunk_handler
                );

            // Request GET reed/status
            coap_init_message(
                request_reed, COAP_TYPE_CON, COAP_GET, 0
                );
            coap_set_header_uri_path(
                request_reed, remote_urls[4]
                );

            COAP_BLOCKING_REQUEST(
                &remote_ep, request_reed, client_chunk_handler
                );

            // Reset timer
            etimer_reset(&timer);
        }

        // Fallback solution 
        // to open door lock
        if (ev == button_hal_release_event)
        {
            // Request POST lock/open
            coap_init_message(
                request_lock, COAP_TYPE_CON, COAP_POST, 0
                );
            coap_set_header_uri_path(
                request_lock, remote_urls[1]
                );

            coap_set_payload(
                request_lock, NULL, 0
                );

            COAP_BLOCKING_REQUEST(
                &remote_ep, request_lock, client_chunk_handler
                );

            // Request POST buzz/sound
            coap_init_message(
                request_buzz, COAP_TYPE_CON, COAP_POST, 0
                );
            coap_set_header_uri_path(
                request_buzz, remote_urls[2]
                );

            coap_set_payload(
                request_buzz, NULL, 0
                );

            COAP_BLOCKING_REQUEST(
                &remote_ep, request_buzz, client_chunk_handler
                );
        }
    }
    
    // Disconnect from remote endpoint
    coap_endpoint_disconnect(&remote_ep);

    PROCESS_END();
}