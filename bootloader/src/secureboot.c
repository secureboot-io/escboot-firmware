#include "bootloader.h"
#include "secureboot.h"
#include "flash.h"
#include "crypto/ecc.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "utils/hexdump.h"

#define FLAG_ENABLED 0x00
#define FLAG_DISABLED 0xFF

secureboot_t __attribute__((section(".secureboot"))) secureboot = {
    .bytes = {0xFF, 0xFF}};

bool securebootVerifyFirmwareSignature(uint8_t *signature)
{
    if (!crypto_ecc_startup())
    {
        return false;
    }
    if (!crypto_ecc_verify((uint8_t*)FLASH_APPLICATION_START_ADDRESS, FLASH_APPLICATION_SIZE, signature, 64, secureboot.devInfo.manufacturerPublicKey, 64))
    {
        crypto_ecc_cleanup();
        return false;
    }
    crypto_ecc_cleanup();
    return true;
}

bool securebootOk()
{
    if(securebootIsProtected())
    {
        if(securebootVerifyFirmwareSignature(secureboot.firmwareSignature)) {
            printf("Firmware signature verified\n");
            return true;
        } else {
            printf("Firmware signature verification failed\n");
            return false;
        }
    }
    else
    {
        printf("Secureboot is disabled\n");
        return true;
    }
}

bool securebootInit()
{
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

bool securebootGetDeviceInfoSignature(uint8_t *buffer)
{
    memcpy(buffer, secureboot.deviceSignature, sizeof(secureboot.deviceSignature));
    return true;
}

bool securebootSetDeviceInfoSignature(uint8_t *buffer)
{
    secureboot_t sb;
    if (!securebootIsProtected())
    {
        return false;
    }
    // Verify signature
    if (!crypto_ecc_startup())
    {
        return false;
    }
    if (!crypto_ecc_verify((uint8_t *)&secureboot.devInfo, sizeof(secureboot.devInfo), buffer, 64, secureboot.devInfo.manufacturerPublicKey, 64))
    {
        printf("Signature verification failed\n");
        crypto_ecc_cleanup();
        return false;
    }
    crypto_ecc_cleanup();
    
    // Write signature
    memcpy(&sb, &secureboot, sizeof(secureboot_t));
    memcpy(sb.deviceSignature, buffer, sizeof(secureboot.deviceSignature));

    flash_write(sb.bytes, sizeof(secureboot_t), (uint32_t)&secureboot);
    // reboot
    bl_reboot();
    return true;
}

bool securebootSetDeviceInfo(deviceInfo_t *deviceInfo)
{
    secureboot_t sb;
    if (securebootIsProtected())
    {
        return false;
    }
    memcpy(&sb, &secureboot, sizeof(secureboot_t));
    memcpy(&sb.devInfo, deviceInfo, SECUREBOOT_DEVICE_INFO_WRITE_SIZE);
    flash_write(sb.bytes, sizeof(secureboot_t), (uint32_t)&secureboot);
    return true;
}

bool securebootSetFirmwareSignature(uint8_t *buffer)
{
    secureboot_t sb;
    if (!securebootIsProtected())
    {
        return false;
    }

    memcpy(&sb, &secureboot, sizeof(secureboot_t));
    memcpy(sb.firmwareSignature, buffer, sizeof(secureboot.firmwareSignature));
    flash_write(sb.bytes, sizeof(secureboot_t), (uint32_t)&secureboot);
    return true;
}

bool securebootGetDeviceInfo(uint8_t *buffer)
{
    memcpy(buffer, &secureboot.devInfo, sizeof(deviceInfo_t));
    return true;
}

bool securebootGetFirmwareSignature(uint8_t *buffer)
{
    memcpy(buffer, secureboot.firmwareSignature, sizeof(secureboot.firmwareSignature));
    return true;
}

bool securebootIsProtected()
{
    return secureboot.protectedFlag == FLAG_ENABLED;
}

bool securebootRead(uint32_t address, uint32_t bufferSize, uint8_t *buffer)
{
    if ((address == SECUREBOOT_PROTECTED_FLAG_ADDRESS) && (bufferSize == SECUREBOOT_PROTECTED_FLAG_SIZE))
    {
        buffer[0] = securebootGetProtectedFlag();
        return true;
    }
    else if ((address == SECUREBOOT_DEVICE_INFO_ADDRESS) && (bufferSize == SECUREBOOT_DEVICE_INFO_SIZE))
    {
        return securebootGetDeviceInfo(buffer);
    }
    else if ((address == SECUREBOOT_DEVICE_SIGNATURE_ADDRESS) && (bufferSize == SECUREBOOT_DEVICE_SIGNATURE_SIZE))
    {
        return securebootGetDeviceInfoSignature(buffer);
    }
    else if ((address == SECUREBOOT_FIRMWARE_SIGNATURE_ADDRESS) && (bufferSize == SECUREBOOT_FIRMWARE_SIGNATURE_SIZE))
    {
        return securebootGetFirmwareSignature(buffer);
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

    // Generate a key pair
    if (!crypto_ecc_startup())
    {
        return false;
    }

    if (!crypto_ecc_generate(sb.devicePrivateKey, sizeof(sb.devicePrivateKey), sb.devInfo.devicePublicKey, sizeof(sb.devInfo.devicePublicKey)))
    {
        crypto_ecc_cleanup();
        return false;
    }

    crypto_ecc_cleanup();

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

bool securebootWrite(uint32_t address, uint32_t bufferSize, uint8_t *buffer)
{
    if ((address == SECUREBOOT_PROTECTED_FLAG_ADDRESS) && (bufferSize == SECUREBOOT_PROTECTED_FLAG_SIZE))
    {
        return securebootSetProtectedFlag(buffer[0]);
    }
    else if ((address == SECUREBOOT_DEVICE_INFO_ADDRESS) && (bufferSize == SECUREBOOT_DEVICE_INFO_WRITE_SIZE))
    {
        return securebootSetDeviceInfo((deviceInfo_t *)buffer);
    }
    else if ((address == SECUREBOOT_DEVICE_SIGNATURE_ADDRESS) && (bufferSize == SECUREBOOT_DEVICE_SIGNATURE_SIZE))
    {
        return securebootSetDeviceInfoSignature(buffer);
    }
    else if ((address == SECUREBOOT_FIRMWARE_SIGNATURE_ADDRESS) && (bufferSize == SECUREBOOT_FIRMWARE_SIGNATURE_SIZE))
    {
        return securebootSetFirmwareSignature(buffer);
    }
    else
    {
        return false;
    }
}
