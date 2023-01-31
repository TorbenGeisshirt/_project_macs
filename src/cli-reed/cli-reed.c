
#include "cli-reed.h"

#include "dev/button-hal.h"

////////////////////////////////////////////////////////////
// OBSERVE
////////////////////////////////////////////////////////////

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

PROCESS_THREAD(cli_reed, ev, data)
{
    static coap_endpoint_t remote_reed_ep;

#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
    //lwm2m_security_init();
#endif

    PROCESS_BEGIN();

    puts(
        "Zolertia RE-Mote: Client - Reed Switch initiated!"
        );

    // Endpoint configuration
    coap_endpoint_parse(
        REMOTE_REED_EP, strlen(REMOTE_REED_EP), &remote_reed_ep
        );

    observee = coap_obs_request_registration(
        &remote_reed_ep, OBS_RESOURCE_URI, observe_callback, NULL
        );

    if (coap_endpoint_is_secure(&remote_reed_ep))
        puts("Confidentiality is protected by DTLS");
    else
        puts("Confidentiality is not protected - DTLS is not enabled!");
    
    // Initialize timer
    etimer_set(
        &dbg_timer, REQUEST_INTERVAL * CLOCK_SECOND
        );

    while (true)
    {
        PROCESS_YIELD();

        if (ev == button_hal_release_event)
        {}

        // DBG status information 
        // in fixed time interval
        if (etimer_expired(&dbg_timer))
        {
            // Reset timer
            etimer_reset(&dbg_timer);
        }
    }
    
    PROCESS_END();
}