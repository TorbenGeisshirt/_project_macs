
#include "hub-gateway.h"

#include "dev/button-hal.h"

#include "../../libjss/jss.h"

////////////////////////////////////////////////////////////
// OBSERVE
////////////////////////////////////////////////////////////

static bool reed_open = false;

static void observe_callback(
    coap_observee_t *obs, 
    void *notification,
    coap_notification_flag_t flag
    )
{
    const uint8_t *payload = NULL;

    int length = 0;
    if (notification)
        length = coap_get_payload(notification, &payload);

    switch(flag)
    {
        case OBSERVE_OK:
            printf("OBSERVE_OK: %*s\n", length, (char*)payload);
            break;
        case NOTIFICATION_OK:
            reed_open = strstr((char*)payload, "\"reed\": 1")
                ? true
                : false;

            printf("NOTIFICATION_OK: %*s\n", length, (char*)payload);
            break;
        case OBSERVE_NOT_SUPPORTED:
            printf("OBSERVE_NOT_SUPPORTED: %*s\n", length, (char*)payload);
            obs = NULL;
            break;
        case ERROR_RESPONSE_CODE:
            printf("ERROR_RESPONSE_CODE: %*s\n", length, (char*)payload);
            obs = NULL;
            break;
        case NO_REPLY_FROM_SERVER:
            printf("NO_REPLY_FROM_SERVER: %*s\n", length, (char*)payload);
            obs = NULL;
            break;
    }
}

////////////////////////////////////////////////////////////

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
    static coap_endpoint_t remote_dlock_ep;
    static coap_endpoint_t remote_buzz_ep;
    static coap_endpoint_t remote_reed_ep;

#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
    //lwm2m_security_init();
#endif

    PROCESS_BEGIN();

    static coap_message_t request_lock[1];
    static coap_message_t request_buzz[1];

    puts(
        "Zolertia RE-Mote: Hub Gateway initiated!"
        );

    // Endpoint configuration
    coap_endpoint_parse(
        REMOTE_LOCK_EP, strlen(REMOTE_LOCK_EP), &remote_dlock_ep
        );
    coap_endpoint_parse(
        REMOTE_BUZZ_EP, strlen(REMOTE_BUZZ_EP), &remote_buzz_ep
        );
    coap_endpoint_parse(
        REMOTE_REED_EP, strlen(REMOTE_REED_EP), &remote_reed_ep
        );

    observee = coap_obs_request_registration(
        &remote_reed_ep, OBS_RESOURCE_URI, observe_callback, NULL
        );

    if (coap_endpoint_is_secure(&remote_dlock_ep))
        puts("Confidentiality is protected by DTLS");
    else
        puts("Confidentiality is not protected - DTLS is not enabled!");
    
    // Connect to remote endpoint
    coap_endpoint_connect(&remote_buzz_ep);
    coap_endpoint_connect(&remote_dlock_ep);

    // Initialize timer
    etimer_set(
        &timer, REQUEST_INTERVAL * CLOCK_SECOND
        );

    while (true)
    {
        PROCESS_WAIT_EVENT();

        // Fallback solution 
        // to open door lock
        if (ev == button_hal_release_event)
        {
            // Request GET lock/open
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

            PROCESS_PAUSE();

            // Request GET buzz/sound
            coap_init_message(
                request_buzz, COAP_TYPE_CON, COAP_GET, 0
                );
            coap_set_header_uri_path(
                request_buzz, remote_urls[2]
                );

            coap_set_payload(
                request_buzz, NULL, 0
                );

            COAP_BLOCKING_REQUEST(
                &remote_buzz_ep, request_buzz, client_chunk_handler
                );
        }

        if (etimer_expired(&timer))
        {
            printf("Reed-Status: %d\n", reed_open);

            // Reset timer
            etimer_reset(&timer);
        }
    }
    
    // Disconnect from remote endpoint
    coap_endpoint_disconnect(&remote_dlock_ep);
    coap_endpoint_disconnect(&remote_buzz_ep);

    PROCESS_END();
}