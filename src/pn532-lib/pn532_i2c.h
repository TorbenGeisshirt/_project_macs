
#ifndef PN532_I2C_H_
#define PN532_I2C_H_

/* Frame Structure:
 * [00][00][FF][LEN][LCS][TFI][PD0][PD1][...][PDn][DCS][00]
 *
 * PREAMBLE         1 byte: 0x00
 * START CODE       2 byte: 0x00,0xFF
 * LEN              1 byte: indicating the number of bytes in the data field (TFI and PD0 to PDn)
 * LCS              1 byte: packet length checksum with [LEN + LCS] = 0x00
 * TFI              1 byte: frame identifier
 *                              - 0xD4 -> transmission from host controller to PN532
 *                              - 0xD5 -> transmission from PN532 to host controller
 * DATA             [LEN - 1] byte: packet data
 *                              - PD0 represents the command code
 * DCS              1 byte: data checksum with [TFI + PD0 + PD1 + ... + PDn + DCS] = 0x00
 * POSTAMBLE        1 byte: 0x00 
*/

#include <stdint.h>
#include <stdbool.h>

#define PN532_ADDR              (0x48 >> 1)

#define PN532_PREAMBLE          (0x00)
#define PN532_STARTCODE1        (0x00)
#define PN532_STARTCODE2        (0xFF)
#define PN532_POSTAMBLE         (0x00)

#define PN532_HOSTTOPN532       (0xD4)
#define PN532_PN532TOHOST       (0xD5)

#define PN532_ACK_WAIT_TIME     (10)

void pn532_i2c_init(
    uint8_t port_sda, uint8_t pin_sda, 
    uint8_t port_scl, uint8_t pin_scl
    );

bool pn532_i2c_write(uint8_t *in_data, uint8_t length);
bool pn532_i2c_read(uint8_t *out_data, uint8_t length, uint16_t timeout);

#endif//PN532_I2C_H_
