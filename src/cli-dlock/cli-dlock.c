
#include "cli-dlock.h"

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

PROCESS_THREAD(cli_dlock, ev, data)
{
    static coap_endpoint_t remote_dlock_ep;

#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
    //lwm2m_security_init();
#endif

    PROCESS_BEGIN();

    static coap_message_t request_lock[1];

    puts(
        "Zolertia RE-Mote: Client - Door Lock initiated!"
        );

    // Endpoint configuration
    coap_endpoint_parse(
        REMOTE_LOCK_EP, strlen(REMOTE_LOCK_EP), &remote_dlock_ep
        );
    
    if (coap_endpoint_is_secure(&remote_dlock_ep))
        puts("Confidentiality is protected by DTLS");
    else
        puts("Confidentiality is not protected - DTLS is not enabled!");
    
    // Connect to remote endpoint
    coap_endpoint_connect(&remote_dlock_ep);

    // Initialize timer
    etimer_set(
        &timer, REQUEST_INTERVAL * CLOCK_SECOND
        );

    while (true)
    {
        PROCESS_YIELD();

        // Fallback solution 
        // to open door lock
        if (ev == button_hal_release_event)
        {
            // Request POST lock/open
            coap_init_message(
                request_lock, COAP_TYPE_CON, COAP_GET, 0
                );
            coap_set_header_uri_path(
                request_lock, remote_urls[1]
                );

            coap_set_payload(
                request_lock, NULL, 0
                );

            COAP_BLOCKING_REQUEST(
                &remote_dlock_ep, request_lock, client_chunk_handler
                );
        }
    }
    
    // Disconnect from remote endpoint
    coap_endpoint_disconnect(&remote_dlock_ep);

    PROCESS_END();
}