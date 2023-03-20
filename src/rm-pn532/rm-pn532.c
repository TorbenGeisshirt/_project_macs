
#include "rm-pn532.h"

#include "contiki.h"

#include "dev/i2c.h"
#include "dev/button-hal.h"

#include "../pn532-lib/pn532.h"

////////////////////////////////////////////////////////////
// RESOURCE - GET HANDLER
////////////////////////////////////////////////////////////

static void res_pn532Read_get_handler(
    coap_message_t  *request, 
    coap_message_t  *response, 
    uint8_t         *buffer, 
    uint16_t         preferred_size, 
    int32_t         *offset
    )
{
    char message[COAP_MAX_CHUNK_SIZE] = "";

    char format[] = "{\"message_id\": %d, \"data\":}";

    int result = snprintf(
        message, (COAP_MAX_CHUNK_SIZE - 1), format, pn532_message_id++
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

PROCESS_THREAD(rm_pn532, ev, data)
{
    PROCESS_BEGIN();

#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
    //lwm2m_security_init();
#endif

    PROCESS_PAUSE();

    printf("Activating resources...\n");
    
    /* Activate CoAP resources */
    coap_activate_resource(&res_pn532Read, "pn532/read");

    pn532_init();

    // Initialize debug timer
    etimer_set(
        &dbg_timer, 3 * CLOCK_SECOND
        );

    while (true) 
    {
        PROCESS_WAIT_EVENT();

        if (ev == button_hal_release_event)
        {
            uint8_t version_data[4];

            bool result = pn532_get_firmware_version(
                            version_data, sizeof(version_data)
                            );

            if (result)
                printf(
                    "Version data: %02X,%02X,%02X,%02X"
                    , version_data[0]
                    , version_data[1]
                    , version_data[2]
                    , version_data[3]
                    );
            else
            {
                printf("Version data could not be received!");
                continue;
            }

            result = pn532_set_sam_configuration();

            if (result)
                printf("\nSAM configuration successful...\n");
            else
            {
                printf("\nSAM configuration not successful...\n");
                continue;
            }
            /*
            uint8_t target[1];

            result = pn532_list_passive_target(target);

            if (result)
                printf("\nTarget found: %d\n", target[0]);
            else
            {
                printf("\nNo target found...\n");
                continue;
            }

            */
            uint8_t uid[4];

            result = pn532_passive_target_uid(
                        uid, sizeof(uid), 1000
                        );

            if (result)
            {
                printf("\nRead UID successful:\n");

                printf("\nUID: %02X", uid[0]);
                for (uint8_t i = 1; i < sizeof(uid); i++)
                    printf("-%02X",uid[i]);
                printf("\n\n");
            }
            else
            {
                printf("\nRead UID not successful!\n");
                continue;
            }

            uint8_t mifare_auth_cmd[12] =
            {
                0x60, // Authentication Type A
                0x07,
                0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,
                uid[0],uid[1],uid[2],uid[3]
            };
            
            /*
            uint8_t mifare_read_cmd[2] =
            {
                0x30, // 
                0x04,
            };
            */

            uint8_t rsp_buffer[1];

            result = pn532_data_exchange(
                        1, // Assuming target is card with logical number 1
                        mifare_auth_cmd, sizeof(mifare_auth_cmd), rsp_buffer, sizeof(rsp_buffer)
                        );

            if (rsp_buffer[0] == 0x00)
                printf("\nAuthentication complete!\n");
            
            /*
            uint8_t rsp_buffer[18];

            result = pn532_data_exchange(
                        1, mifare_read_cmd, sizeof(mifare_read_cmd), rsp_buffer, sizeof(rsp_buffer)
                        );
            */
            
            if (result)
            {
                printf("\nData exchange successful:\n");

                printf("\nResponse Buffer: %02X", rsp_buffer[0]);
                for (uint8_t i = 1; i < sizeof(rsp_buffer); i++)
                    printf(",%02X",rsp_buffer[i]);
                printf("\n\n");
            }
            else
            {
                printf("\nData exchange not successful!\n");
                continue;
            }
        }
    }

    PROCESS_END();
}
