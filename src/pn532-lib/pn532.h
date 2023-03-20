
#ifndef PN532_H_
#define PN532_H_

#include <stdint.h>
#include <stdbool.h>

#define PN532_CMD_GETFIRMWAREVERSION    (0x02)
#define PN532_CMD_SAMCONFIGURATION      (0x14)
#define PN532_CMD_INLISTPASSIVETARGET   (0x4A)
#define PN532_CMD_INDATAEXCHANGE        (0x40)

#define PN532_RSP_INDATAEXCHANGE        (0x41)
#define PN532_RSP_INLISTPASSIVETARGET   (0x4B)

#define PN532_MIFARE_ISO14443A          (0x00)

#define MIFARE_CMD_AUTH_A               (0x60)
#define MIFARE_CMD_AUTH_B               (0x61)
#define MIFARE_CMD_READ                 (0x30)
#define MIFARE_CMD_WRITE                (0xA0)

/// @brief 
///     Initialize PN532 interface.
void pn532_init();

/// @brief 
///     The PN532 sends back the version of the embedded firmware.
/// @param version_data 
///     4 byte of version data
///     [IC][VERSION][REVISION][SUPPORT]
/// @return 
///     true in case command succeeds; otherwise false
bool pn532_get_firmware_version(uint8_t *version_data, uint8_t length);

/// @brief 
///     Perform data flow path selection.
/// @return 
///     true in case command succeeds; otherwise false
bool pn532_set_sam_configuration(void);

/// @brief 
///     Detect card/tag in passive mode.
/// @param target 
///     Store logical card/tag number
/// @return 
///     true in case command succeeds; otherwise false
bool pn532_list_passive_target(uint8_t *target);

/// @brief 
///     Protocol data exchange between PN532 and target card/tag.
/// @param target
///     Locical target number
/// @param cmd
///     Command send to card/tag
/// @param cmd_length
///     Command array length
/// @param rsp 
///     Buffer to store response
/// @param rsp_length 
///     Response buffer length
/// @return 
///     true in case command succeeds; otherwise false
bool pn532_data_exchange(
    uint8_t target, uint8_t *cmd, uint8_t cmd_length, uint8_t *rsp, uint8_t rsp_length
    );

/// @brief 
///     Read UId from target card/tag.
/// @param uid
///     Buffer to store target UID; consists of 4 byte for mifare classic cards/tags
/// @param uid_length
///     Length of UID array
/// @param timeout
///     Allowed message timeout
/// @return 
///     true in case command succeeds; otherwise false
bool pn532_passive_target_uid(uint8_t *uid, uint8_t uid_length, uint16_t timeout);

#endif//PN532_H_
