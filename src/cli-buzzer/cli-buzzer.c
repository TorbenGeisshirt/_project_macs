
#include "cli-buzzer.h"

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

PROCESS_THREAD(cli_buzzer, ev, data)
{
    static coap_endpoint_t remote_buzz_ep;

#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
    //lwm2m_security_init();
#endif

    PROCESS_BEGIN();

    static coap_message_t request_buzz[1];

    puts(
        "Zolertia RE-Mote: Client - Piezo Buzzer initiated!"
        );

    // Endpoint configuration
    coap_endpoint_parse(
        REMOTE_BUZZ_EP, strlen(REMOTE_BUZZ_EP), &remote_buzz_ep
        );

    if (coap_endpoint_is_secure(&remote_buzz_ep))
        puts("Confidentiality is protected by DTLS");
    else
        puts("Confidentiality is not protected - DTLS is not enabled!");
    
    // Connect to remote endpoint
    coap_endpoint_connect(&remote_buzz_ep);

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
            // Request GET buzz/sound
            coap_init_message(
                request_buzz, COAP_TYPE_CON, COAP_GET, 0
                );
            coap_set_header_uri_path(
                request_buzz, remote_urls[1]
                );

            coap_set_payload(
                request_buzz, NULL, 0
                );

            COAP_BLOCKING_REQUEST(
                &remote_buzz_ep, request_buzz, client_chunk_handler
                );
        }
    }
    
    // Disconnect from remote endpoint
    coap_endpoint_disconnect(&remote_buzz_ep);

    PROCESS_END();
}