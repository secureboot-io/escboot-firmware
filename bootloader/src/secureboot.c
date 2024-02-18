#include "bootloader.h"
#include "secureboot.h"
#include "flash.h"
#include "crypto/ecc.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define FLAG_ENABLED 0x00
#define FLAG_DISABLED 0xFF

secureboot_t __attribute__((section(".secureboot"))) secureboot = {
    .bytes = {0xFF, 0xFF}};

void crypto_ecc_test();

bool securebootInit()
{
    // Initialize ecc
    bool ret = crypto_ecc_startup();
    if (!ret)
    {
        printf("Unable to start cryptograpohy engine\n");
        return false;
    }

    // Test ecc, this will be removed later
    crypto_ecc_test();
    
    crypto_ecc_cleanup();
    return true;
}

uint8_t securebootGetProtectedFlag()
{
    return secureboot.protectedFlag;
}

bool securebootSetProtectedFlag(uint8_t flag)
{
    if (secureboot.protectedFlag == flag)
    {
        return true;
    }
    else if (flag == FLAG_ENABLED)
    {
        securebootProtect();
        return true;
    }
    else if (flag == FLAG_DISABLED)
    {
        securebootUnprotect();
        return true;
    }
    else
    {
        return false;
    }
}

bool securebootSetDeviceInfo(deviceInfo_t *deviceInfo)
{
    secureboot_t sb;
    if (securebootIsProtected())
    {
        return false;
    }
    memcpy(&sb, &secureboot, sizeof(secureboot_t));
    memcpy(&sb.devInfo, deviceInfo, sizeof(deviceInfo_t));
    flash_write(sb.bytes, sizeof(secureboot_t), (uint32_t)&secureboot);
    return true;
}

bool securebootGetDeviceInfo(uint8_t *buffer)
{
    memcpy(buffer, &secureboot.devInfo, sizeof(deviceInfo_t));
    return true;
}

uint8_t securebootGetEnabledFlag()
{
    return secureboot.enabledFlag;
}

bool securebootSetEnabledFlag(uint8_t flag)
{
    if(!securebootIsProtected())
    {
        return false;
    }
    if (secureboot.enabledFlag == flag)
    {
        return true;
    }
    else if (flag == FLAG_ENABLED)
    {
        securebootEnable();
        return true;
    }
    else if (flag == FLAG_DISABLED)
    {
        securebootDisable();
        return true;
    }
    else
    {
        return false;
    }
}

bool securebootIsProtected()
{
    return secureboot.protectedFlag == FLAG_ENABLED;
}

bool securebootIsEnabled()
{
    return secureboot.enabledFlag == FLAG_ENABLED;
}

bool securebootRead(uint32_t address, uint32_t bufferSize, uint8_t *buffer)
{
    if ((address == SECUREBOOT_PROTECTED_FLAG_ADDRESS) && (bufferSize == SECUREBOOT_PROTECTED_FLAG_SIZE))
    {
        buffer[0] = securebootGetProtectedFlag();
        return true;
    }
    else if ((address == SECUREBOOT_ENABLED_FLAG_ADDRESS) && (bufferSize == SECUREBOOT_ENABLED_FLAG_SIZE))
    {
        buffer[0] = securebootGetEnabledFlag();
        return true;
    }
    else if ((address == SECUREBOOT_DEVICE_INFO_ADDRESS) && (bufferSize == SECUREBOOT_DEVICE_INFO_SIZE))
    {
        return securebootGetDeviceInfo(buffer);
    }
    else
    {
        return false;
    }
}

bool securebootProtect()
{
    secureboot_t sb;
    memcpy(&sb, &secureboot, sizeof(secureboot_t));

    sb.protectedFlag = FLAG_ENABLED;
    sb.enabledFlag = FLAG_ENABLED;
    
    flash_write(sb.bytes, sizeof(secureboot_t), (uint32_t)&secureboot);
    
    bl_reboot();
    return true;
}

bool securebootUnprotect()
{
    flash_erase_page((uint32_t)&secureboot);
    bl_reboot();
    return true;
}

bool securebootEnable()
{
    secureboot_t sb;

    memcpy(&sb, &secureboot, sizeof(secureboot_t));
    sb.enabledFlag = FLAG_ENABLED;
    
    flash_write((uint8_t *)&sb, SECUREBOOT_ENABLED_FLAG_SIZE, SECUREBOOT_ENABLED_FLAG_ADDRESS);
    return true;
}

bool securebootDisable()
{
    secureboot_t sb;

    memcpy(&sb, &secureboot, sizeof(secureboot_t));
    sb.enabledFlag = FLAG_DISABLED;
    
    flash_write((uint8_t*)&sb, SECUREBOOT_ENABLED_FLAG_SIZE, SECUREBOOT_ENABLED_FLAG_ADDRESS);
    return true;
}

bool securebootWrite(uint32_t address, uint32_t bufferSize, uint8_t *buffer)
{
    if ((address == SECUREBOOT_PROTECTED_FLAG_ADDRESS) && (bufferSize == SECUREBOOT_PROTECTED_FLAG_SIZE))
    {
        return securebootSetProtectedFlag(buffer[0]);
    }
    else if ((address == SECUREBOOT_ENABLED_FLAG_ADDRESS) && (bufferSize == SECUREBOOT_ENABLED_FLAG_SIZE))
    {
        return securebootSetEnabledFlag(buffer[0]);
    }
    else if ((address == SECUREBOOT_DEVICE_INFO_ADDRESS) && (bufferSize == SECUREBOOT_DEVICE_INFO_SIZE))
    {
        return securebootSetDeviceInfo((deviceInfo_t *)buffer);
    }
    else
    {
        return false;
    }
}

