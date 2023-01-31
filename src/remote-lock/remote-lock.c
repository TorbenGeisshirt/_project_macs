
#include "remote-lock.h"
#include "contiki.h"
#include "dev/gpio.h"
#include "dev/i2c.h"

#include "dev/button-hal.h"

#include <stdbool.h>

#define I2C_SCL_PORT    GPIO_C_NUM
#define I2C_SCL_PIN     3
#define I2C_SDA_PORT    GPIO_C_NUM
#define I2C_SDA_PIN     2

#define PN532_I2C_ADDRESS (0x48 >> 1)

#define GPIO_A_NUM 0

#define EXAMPLE_PORT_BASE   GPIO_PORT_TO_BASE(GPIO_A_NUM)
#define EXAMPLE_PIN_MASK    GPIO_PIN_MASK(5)

void pn532_i2c_read(uint8_t address, uint8_t *data, uint16_t len)
{
    i2c_master_enable();
    i2c_master_set_slave_address(PN532_I2C_ADDRESS, 1);
    
    printf("\n-------- Pre-Burst --------\n");
    printf("Busy?: %d\n", i2c_master_busy());
    printf("Err?: %d\n", i2c_master_error());

    uint8_t result = i2c_burst_receive(address, data, len);

    printf("\n-------- Post-Burst --------\n");
    printf("Busy?: %d\n", i2c_master_busy());
    printf("Err?: %d\n", i2c_master_error());

    if(result == I2C_MASTER_ERR_NONE) 
    {
        printf("PM: Data 0x%02X\n", *data);
    }
    else
    {
        printf("%d", result);
    }

    i2c_master_disable();
}

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

    i2c_init(
        I2C_SDA_PORT, I2C_SDA_PIN, 
        I2C_SCL_PORT, I2C_SCL_PIN,
        I2C_SCL_FAST_BUS_SPEED
        );

    printf("\n-------- Init I²C --------\n");
    printf("Busy?: %d\n", i2c_master_busy());
    printf("Err?: %d\n", i2c_master_error());

    GPIO_SOFTWARE_CONTROL(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);

    

    GPIO_SET_OUTPUT(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);
    GPIO_CLR_PIN(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);
    
    while (true) 
    {
        PROCESS_WAIT_EVENT();

        if (ev == button_hal_release_event)
        {
            uint8_t response[16];

            pn532_i2c_read(
                PN532_I2C_ADDRESS, response, sizeof(response)
                );

            uint8_t res = GPIO_READ_PIN(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);

            if (GPIO_READ_PIN(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK))
                GPIO_CLR_PIN(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);
            else
            {
                GPIO_SET_PIN(EXAMPLE_PORT_BASE, EXAMPLE_PIN_MASK);
            }

            printf("\n\nPin: %d\n\n", res);
        }
    }

    PROCESS_END();
}
