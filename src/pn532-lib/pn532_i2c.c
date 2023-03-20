
#include "pn532_i2c.h"

#include "contiki.h"
#include "contiki-conf.h"

#include "dev/i2c.h"

#include <string.h>

#define DBG(MSG)                \
            printf(MSG)

#define DBGA(MSG,ARG)           \
            printf(MSG,ARG);

bool pn532_i2c_get_ack_frame()
{
    DBG("Begin I2C Wait for ACK...\n");

    static unsigned long time_start;
    
    const uint8_t PN532_ACK[] = 
    {
        0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00
    };

    uint8_t buffer[
        sizeof(PN532_ACK) + 1
        ];

    uint8_t tries = 0;

    do 
    {
        uint8_t merror = i2c_single_receive(
                            PN532_ADDR, buffer
                            );

        if ( merror != I2C_MASTER_ERR_NONE )
            return false;

        if ( buffer[0] & 0x01 )
            break;

        tries++;
        time_start = clock_time();
        while (clock_time() - time_start < (CLOCK_SECOND / 10));

        if ( tries > PN532_ACK_WAIT_TIME )
            return false;
    } 
    while( true );

    uint8_t merror = i2c_burst_receive(
                        PN532_ADDR, buffer, sizeof(buffer)
                        );

    if (merror != I2C_MASTER_ERR_NONE)
        return false;

    DBGA("\nACK Buffer: %02X",buffer[0]);
    for (uint8_t i = 1; i < sizeof(buffer); i++)
        DBGA(",%02X",buffer[i]);
    DBG("\n\n");

    uint8_t *lhs = &buffer[1];

    if ( memcmp( lhs, PN532_ACK, sizeof(PN532_ACK) ) == 0 )
        return true;

    return false;
}

uint8_t pn532_i2c_read_length(uint16_t timeout)
{
    DBG("Begin I2C Response-Read-LEN Operation...\n");

    static unsigned long time_start;

    uint8_t PN532_NACK[] = 
    {
        0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00
    };

    uint8_t buffer[
        sizeof(PN532_NACK) + 1
        ];

    uint8_t tries = 0;

    do
    {
        uint8_t merror = i2c_single_receive(
                            PN532_ADDR, buffer
                            );

        if ( merror != I2C_MASTER_ERR_NONE )
            return 0;

        if ( buffer[0] & 0x01 )
            break;

        tries++;
        time_start = clock_time();
        while (clock_time() - time_start < (CLOCK_SECOND / 10));

        if ( tries > PN532_ACK_WAIT_TIME )
            return 0;
    } 
    while (true);

    uint8_t merror = i2c_burst_receive(
                        PN532_ADDR, buffer, sizeof(buffer)
                        );

    DBGA("\nResponse-Length Buffer: %02X",buffer[0]);
    for (uint8_t i = 1; i < sizeof(buffer); i++)
        DBGA(",%02X",buffer[i]);
    DBG("\n\n");

    if (merror != I2C_MASTER_ERR_NONE)
        return 0;
    
    if (buffer[1] != PN532_PREAMBLE     ||
        buffer[2] != PN532_STARTCODE1   ||
        buffer[3] != PN532_STARTCODE2
        ) 
    {
        return 0;
    }

    uint8_t result = buffer[4];

    merror = i2c_burst_send(
                PN532_ADDR, PN532_NACK, sizeof(PN532_NACK)
                );

    if (merror != I2C_MASTER_ERR_NONE)
        return 0;

    return result;
}

void pn532_i2c_init(
    uint8_t port_sda, uint8_t pin_sda, 
    uint8_t port_scl, uint8_t pin_scl
    )
{
    DBG("Start I2C Initialization\n");
    i2c_init(port_sda, pin_sda, port_scl, pin_scl, I2C_SCL_FAST_BUS_SPEED);
    DBG("End I2C Initialization\n");
}

bool pn532_i2c_write(uint8_t *in_data, uint8_t length)
{
    DBG("Begin I2C Command-Write Operation...\n");

    uint8_t buffer[length+8];
        buffer[0] = PN532_PREAMBLE;
        buffer[1] = PN532_STARTCODE1;
        buffer[2] = PN532_STARTCODE2;

    uint8_t LEN = length+1;
        buffer[3] = LEN;

    uint8_t LCS = ~LEN + 1;
        buffer[4] = LCS;
        buffer[5] = PN532_HOSTTOPN532;

    uint8_t DCS = PN532_HOSTTOPN532;

    for (uint8_t i = 0; i < length; i++)
    {
        buffer[i+6] = in_data[i];
        DCS        += in_data[i];
    }

    buffer[sizeof(buffer)-2] = ~DCS + 1;
    buffer[sizeof(buffer)-1] = PN532_POSTAMBLE;

    DBGA("\nCommand Buffer: %02X",buffer[0]);
    for (uint8_t i = 1; i < sizeof(buffer); i++)
        DBGA(",%02X",buffer[i]);
    DBG("\n\n");

    uint8_t merror = i2c_burst_send(
                        PN532_ADDR, buffer, sizeof(buffer)
                        );

    if ( merror != I2C_MASTER_ERR_NONE )
        return false;

    return pn532_i2c_get_ack_frame();
}

bool pn532_i2c_read(uint8_t *out_data, uint8_t length, uint16_t timeout)
{
    DBG("Begin I2C Response-Read Operation...\n");

    static unsigned long time_start;

    uint8_t LEN = pn532_i2c_read_length(
        timeout
        );

    uint8_t buffer[
        6 + LEN + 2 + 1
        ];

    uint8_t tries = 0;

    do 
    {
        uint8_t merror = i2c_single_receive(
                            PN532_ADDR, buffer
                            );

        if ( merror != I2C_MASTER_ERR_NONE )
            return 0;

        if ( buffer[0] & 0x01 )
            break;

        tries++;
        time_start = clock_time();
        while (clock_time() - time_start < (CLOCK_SECOND / 10));

        if ( tries > timeout / PN532_ACK_WAIT_TIME )
            return 0;
    }
    while (true);

    uint8_t merror = i2c_burst_receive(
                        PN532_ADDR, buffer, sizeof(buffer)
                        );

    if (merror != I2C_MASTER_ERR_NONE)
        return 0;
    

    DBGA("\nResponse Buffer: %02X",buffer[0]);
    for (uint8_t i = 1; i < sizeof(buffer); i++)
        DBGA(",%02X",buffer[i]);
    DBG("\n\n");

    if (buffer[1] != PN532_PREAMBLE     ||
        buffer[2] != PN532_STARTCODE1   ||
        buffer[3] != PN532_STARTCODE2
        ) 
    {
        return 0;
    }

    // TODO: Add error checks via checksum

    for (uint8_t i = 0; i < length; i++)
        out_data[i] = buffer[i+6+2];

    return true;
}
