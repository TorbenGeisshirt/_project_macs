
#include "pn532.h"
#include "pn532_i2c.h"

#include "dev/gpio.h"

#define I2C_INT_PORT    GPIO_D_NUM
#define I2C_INT_PIN     0
#define I2C_SDA_PORT    GPIO_C_NUM
#define I2C_SDA_PIN     2
#define I2C_SCL_PORT    GPIO_C_NUM
#define I2C_SCL_PIN     3

void pn532_init()
{
    pn532_i2c_init(
        I2C_SDA_PORT, I2C_SDA_PIN, 
        I2C_SCL_PORT, I2C_SCL_PIN
        );
}

bool pn532_get_firmware_version(uint8_t *version_data, uint8_t length)
{
    uint8_t cmd_buffer[] = 
    {
        PN532_CMD_GETFIRMWAREVERSION
    };

    if (pn532_i2c_write(cmd_buffer, sizeof(cmd_buffer)))
        return pn532_i2c_read(version_data, length, 100);

    return false;
}

bool pn532_set_sam_configuration(void)
{
    uint8_t cmd_buffer[] = 
    {
        PN532_CMD_SAMCONFIGURATION, 
        0x01, // Set normal mode
        0x14, // timeout 50ms * 20 = 1 second
        0x01  // use IRQ
    };

    return pn532_i2c_write(cmd_buffer, sizeof(cmd_buffer));
}

bool pn532_list_passive_target(uint8_t *target)
{
    uint8_t cmd_buffer[] = 
    {
        PN532_CMD_INLISTPASSIVETARGET, 
        0x01, // Target count
        0x00  // Baud Rate -> 106 kbps
    };

    if (pn532_i2c_write(cmd_buffer, sizeof(cmd_buffer)))
        return pn532_i2c_read(target, 1, 1000);

    return false;
}

bool pn532_data_exchange(
    uint8_t target, uint8_t *cmd, uint8_t cmd_length, uint8_t *rsp, uint8_t rsp_length
    )
{
    uint8_t cmd_buffer[16] = 
    {
        PN532_CMD_INDATAEXCHANGE, 
        target
    };

    for (uint8_t i = 0; i < cmd_length; i++)
        cmd_buffer[i+2] = cmd[i];

    if (pn532_i2c_write(cmd_buffer, cmd_length+2))
        return pn532_i2c_read(rsp, rsp_length, 1000);

    return false;
}

bool pn532_passive_target_uid(uint8_t *uid, uint8_t uid_length, uint16_t timeout)
{
    uint8_t cmd_buffer[] = 
    {
        PN532_CMD_INLISTPASSIVETARGET, 
        0x01, // Target count
        0x00  // Baud Rate -> 106 kbps
    };

    uint8_t buffer[10];

    if (pn532_i2c_write(cmd_buffer, sizeof(cmd_buffer)))
    {
        if (pn532_i2c_read(buffer, sizeof(buffer), timeout) == false)
            return false;

        if (buffer[0] != 1)
            return false;

        printf("\nLogical Number:   %02X",                  buffer[1]);
        printf("\nSENS_RES:         %02X,%02X",             buffer[2], buffer[3]);
        printf("\nSEL_RES:          %02X",                  buffer[4]);
        printf("\nUID Length:       %02X",                  buffer[5]);
        printf("\nUID Content:      %02X-%02X-%02X-%02X",   buffer[6],buffer[7],buffer[8],buffer[9]);

        if (buffer[5] != uid_length)
            return false;

        for (uint8_t i = 0; i < uid_length; i++)
            uid[i] = buffer[i+6];

        return true;
    }

    return false;
}
