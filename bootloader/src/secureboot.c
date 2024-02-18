#include "bootloader.h"
#include "secureboot.h"
#include "flash.h"
#include "crypto/ecc.h"
#include <stdio.h>

#define FLAG_ENABLED 0x00
#define FLAG_DISABLED 0xFF

typedef struct __attribute__((packed))
{
    uint8_t manufacturerId[32];
    uint8_t deviceId[32];
    uint8_t manufacturerPublicKey[64];

} deviceInfo_t;

typedef union __attribute__((packed))
{
    uint8_t bytes[1024];
    uint16_t words[512];
    uint32_t dwords[256];
    struct
    {
        uint8_t secureFlag;
        uint8_t enabledFlag;
        deviceInfo_t devInfo;
        uint8_t devicePublicKey[64];
        uint8_t deviceSignature[64];
        uint8_t devicePrivateKey[32];
        uint8_t firmwareSignature[64];
    };
} secureboot_t;

secureboot_t __attribute__((section(".secureboot"))) secureboot = {
    .bytes = {0xFF, 0xFF}};

void crypto_ecc_test();
bool secureboot_startup()
{
    bool ret = crypto_ecc_startup();
    if (!ret) {
        printf("Unable to start cryptograpohy engine\n");
        return false;
    }
    crypto_ecc_test();
    crypto_ecc_cleanup();
    return true;
}

bool secureboot_read(uint32_t address, uint32_t bufferSize, uint8_t *buffer)
{
    if ((address == SECUREBOOT_SECURE_FLAG_ADDRESS) && (bufferSize == SECUREBOOT_SECURE_FLAG_SIZE))
    {
        buffer[0] = secureboot.secureFlag;
        return true;
    }
    else if ((address == SECUREBOOT_ENABLED_FLAG_ADDRESS) && (bufferSize == SECUREBOOT_ENABLED_FLAG_SIZE))
    {
        buffer[0] = secureboot.enabledFlag;
        return true;
    }
    else
    {
        return false;
    }
}

bool secureboot_init()
{
    secureboot_t sb;
    sb.secureFlag = FLAG_ENABLED;
    sb.enabledFlag = FLAG_ENABLED;
    flash_write(sb.bytes, sizeof(secureboot_t), (uint32_t)&secureboot);
    bl_reboot();
    return true;
}

bool secureboot_deinit()
{
    flash_erase_page((uint32_t)&secureboot);
    bl_reboot();
    return true;
}

bool secureboot_write(uint32_t address, uint32_t bufferSize, uint8_t *buffer)
{
    if ((address == SECUREBOOT_SECURE_FLAG_ADDRESS) && (bufferSize == SECUREBOOT_SECURE_FLAG_SIZE))
    {
        // Chnaging secure flag
        if (secureboot.secureFlag == buffer[0])
        {
            // no change
            return true;
        }
        else if (buffer[0] == FLAG_ENABLED)
        {
            // enabling secure boot
            secureboot_init();
            return true;
        }
        else if (buffer[0] == FLAG_DISABLED)
        {
            // disabling secure boot
            secureboot_deinit();
            return true;
        }
        else
        {
            // invalid value
            return false;
        }
        return true;
    }
    else if ((address == SECUREBOOT_ENABLED_FLAG_ADDRESS) && (bufferSize == SECUREBOOT_ENABLED_FLAG_SIZE))
    {
        secureboot.enabledFlag = buffer[0];
        return true;
    }
    else
    {
        return false;
    }
}

// bool secureboot_enable()
// {
//     uint8_t enabledFlag = 0x7c;
//     return secureboot_write(SECUREBOOT_ENABLED_FLAG_ADDRESS, SECUREBOOT_ENABLED_FLAG_SIZE, &enabledFlag);
// }

// bool secureboot_disable()
// {
//     uint8_t enabledFlag = 0x00;
//     return secureboot_write(SECUREBOOT_ENABLED_FLAG_ADDRESS, SECUREBOOT_ENABLED_FLAG_SIZE, &enabledFlag);
// }

// bool secureboot_verify(uint32_t address, uint32_t bufferSize, uint8_t *buffer)
// {
//     return true;
// }