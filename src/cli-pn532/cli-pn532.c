
#include "cli-pn532.h"

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

PROCESS_THREAD(cli_pn532, ev, data)
{
    static coap_endpoint_t remote_pn532_ep;

#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
    //lwm2m_security_init();
#endif

    PROCESS_BEGIN();

    static coap_message_t request_pn532Read[1];

    puts(
        "Zolertia RE-Mote: Client - PN532 RFID/NFC initiated!"
        );

    // Endpoint configuration
    coap_endpoint_parse(
        REMOTE_PN532_EP, strlen(REMOTE_PN532_EP), &remote_pn532_ep
        );

    if (coap_endpoint_is_secure(&remote_pn532_ep))
        puts("Confidentiality is protected by DTLS");
    else
        puts("Confidentiality is not protected - DTLS is not enabled!");
    
    // Connect to remote endpoint
    coap_endpoint_connect(&remote_pn532_ep);

    // Initialize timer
    etimer_set(
        &timer, REQUEST_INTERVAL * CLOCK_SECOND
        );

    while (true)
    {
        PROCESS_YIELD();

        if (ev == button_hal_release_event)
        {
            // Request GET pn532/read
            coap_init_message(
                request_pn532Read, COAP_TYPE_CON, COAP_GET, 0
                );
            coap_set_header_uri_path(
                request_pn532Read, remote_urls[1]
                );

            coap_set_payload(
                request_pn532Read, NULL, 0
                );

            COAP_BLOCKING_REQUEST(
                &remote_pn532_ep, request_pn532Read, client_chunk_handler
                );
        }
    }
    
    // Disconnect from remote endpoint
    coap_endpoint_disconnect(&remote_pn532_ep);

    PROCESS_END();
}