
#include "remote-lock.h"

PROCESS_THREAD(remote_lock, ev, data)
{
    PROCESS_BEGIN();

#ifdef COAP_DTLS_KEYSTORE_CONF_WITH_LWM2M
    //lwm2m_security_init();
#endif

    PROCESS_PAUSE();
    
    printf("Activating resources\n");
    
    /* Activate CoAP resources */
    coap_activate_resource(&res_reed, "reed");
    coap_activate_resource(&res_lock, "lock/open");
    coap_activate_resource(&res_buzzer, "buzzer/toggle");

    while (true) 
    {
        PROCESS_WAIT_EVENT();
    }

    PROCESS_END();
}
