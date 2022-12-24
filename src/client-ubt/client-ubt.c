
#include "client-ubt.h"

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

PROCESS_THREAD(client_ubt, ev, data)
{
    //button_hal_button_t *user_btn;
    static coap_endpoint_t remote_ep;

#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
    //lwm2m_security_init();
#endif

    PROCESS_BEGIN();

    static coap_message_t request[1];

    puts(
        "Re-Mote: Client UBt started!"
        );

    //user_btn = button_hal_get_by_index(0);

    // Endpoint configuration
    coap_endpoint_parse(
        REMOTE_EP, strlen(REMOTE_EP), &remote_ep
        );

    puts(
        "Server address:"
        );

    //LOG_INFO_COAP_EP(&remote_ep);
    //LOG_INFO_("\n");

    if (coap_endpoint_is_secure(&remote_ep))
        puts("Confidentiality is protected by DTLS");
    else
        puts("Confidentiality is not protected - DTLS is not enabled");
    
    // Connect to remote endpoint
    coap_endpoint_connect(&remote_ep);

    // Initialize timer
    etimer_set(&timer, REQUEST_INTERVAL * CLOCK_SECOND);

    while (true)
    {
        PROCESS_YIELD();

        if (etimer_expired(&timer))
        {
            // Request GET lock/open
            coap_init_message(
                request, COAP_TYPE_CON, COAP_GET, 0
                );
            coap_set_header_uri_path(
                request, remote_urls[2]
                );

            COAP_BLOCKING_REQUEST(
                &remote_ep, request, client_chunk_handler
                );

            // Request GET buzzer/toggle
            coap_init_message(
                request, COAP_TYPE_CON, COAP_GET, 0
                );
            coap_set_header_uri_path(
                request, remote_urls[3]
                );

            COAP_BLOCKING_REQUEST(
                &remote_ep, request, client_chunk_handler
                );

            // Reset timer
            etimer_reset(&timer);
        }
        else if (ev == button_hal_release_event)
        {
            // Request POST lock/open
            coap_init_message(
                request, COAP_TYPE_CON, COAP_POST, 0
                );
            coap_set_header_uri_path(
                request, remote_urls[2]
                );

            const char logOpen[] = "Log: Request resource \"lock/open\"";

            coap_set_payload(
                request, (uint8_t *)logOpen, sizeof(logOpen) - 1
                );

            COAP_BLOCKING_REQUEST(
                &remote_ep, request, client_chunk_handler
                );

            // Request POST buzzer/toggle
            coap_init_message(
                request, COAP_TYPE_CON, COAP_POST, 0
                );
            coap_set_header_uri_path(
                request, remote_urls[3]
                );

            const char logToggle[] = "Log: Request resource \"buzzer/toggle\"";

            coap_set_payload(
                request, (uint8_t *)logToggle, sizeof(logToggle) - 1
                );

            COAP_BLOCKING_REQUEST(
                &remote_ep, request, client_chunk_handler
                );
        }
    }
    
    // Disconnect from remote endpoint
    coap_endpoint_disconnect(&remote_ep);

    PROCESS_END();
}