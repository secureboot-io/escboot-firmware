#include "bootloader.h"
#include "secureboot.h"
#include "flash.h"
#include "crypto/ecc.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
// #include "utils/hexdump.h"

#define FLAG_ENABLED 0x00
#define FLAG_DISABLED 0xFF

secureboot_t __attribute__((section(".secureboot"))) secureboot = {
    .bytes = {0xFF, 0xFF}};

bool sbIsFirmwareSignatureValid(uint8_t *signature)
{
    if (!cryptoInitECC())
    {
        return false;
    }
    if (!cryptoVerifyECC((uint8_t*)FLASH_APPLICATION_START_ADDRESS, FLASH_APPLICATION_SIZE, signature, 64, secureboot.devInfo.manufacturerPublicKey, 64))
    {
        cryptoCleanECC();
        return false;
    }
    cryptoCleanECC();
    return true;
}

bool sbOk()
{
    if(sbIsProtected())
    {
        if(sbIsFirmwareSignatureValid(secureboot.firmwareSignature)) {
            // printf("Firmware signature verified\n");
            return true;
        } else {
            // printf("Firmware signature verification failed\n");
            return false;
        }
    }
    else
    {
        // printf("Secureboot is disabled\n");
        return true;
    }
}

bool sbInit()
{
    return true;
}

uint8_t sbGetProtectedFlag()
{
    return secureboot.protectedFlag;
}

bool sbSetProtectedFlag(uint8_t flag)
{
    if (secureboot.protectedFlag == flag)
    {
        return true;
    }
    else if (flag == FLAG_ENABLED)
    {
        sbProtect();
        return true;
    }
    else if (flag == FLAG_DISABLED)
    {
        sbUnprotect();
        return true;
    }
    else
    {
        return false;
    }
}

bool sbGetDeviceInfoSignature(uint8_t *buffer)
{
    memcpy(buffer, secureboot.deviceSignature, sizeof(secureboot.deviceSignature));
    return true;
}

bool sbSetDeviceInfoSignature(uint8_t *buffer)
{
    secureboot_t sb;
    if (!sbIsProtected())
    {
        return false;
    }
    // Verify signature
    if (!cryptoInitECC())
    {
        return false;
    }
    if (!cryptoVerifyECC((uint8_t *)&secureboot.devInfo, sizeof(secureboot.devInfo), buffer, 64, secureboot.devInfo.manufacturerPublicKey, 64))
    {
        // printf("Signature verification failed\n");
        cryptoCleanECC();
        return false;
    }
    cryptoCleanECC();
    
    // Write signature
    memcpy(&sb, &secureboot, sizeof(secureboot_t));
    memcpy(sb.deviceSignature, buffer, sizeof(secureboot.deviceSignature));

    flWrite(sb.bytes, sizeof(secureboot_t), (uint32_t)&secureboot);
    // reboot
    blRequestReboot();
    return true;
}

bool sbSetDeviceInfo(deviceInfo_t *deviceInfo)
{
    secureboot_t sb;
    if (sbIsProtected())
    {
        return false;
    }
    memcpy(&sb, &secureboot, sizeof(secureboot_t));
    memcpy(&sb.devInfo, deviceInfo, SECUREBOOT_DEVICE_INFO_WRITE_SIZE);
    flWrite(sb.bytes, sizeof(secureboot_t), (uint32_t)&secureboot);
    return true;
}

bool sbSetFirmwareSignature(uint8_t *buffer)
{
    secureboot_t sb;
    if (!sbIsProtected())
    {
        return false;
    }

    memcpy(&sb, &secureboot, sizeof(secureboot_t));
    memcpy(sb.firmwareSignature, buffer, sizeof(secureboot.firmwareSignature));
    flWrite(sb.bytes, sizeof(secureboot_t), (uint32_t)&secureboot);
    return true;
}

bool sbGetDeviceInfo(uint8_t *buffer)
{
    memcpy(buffer, &secureboot.devInfo, sizeof(deviceInfo_t));
    return true;
}

bool sbGetFirmwareSignature(uint8_t *buffer)
{
    memcpy(buffer, secureboot.firmwareSignature, sizeof(secureboot.firmwareSignature));
    return true;
}

bool sbIsProtected()
{
    return secureboot.protectedFlag == FLAG_ENABLED;
}

bool sbRead(uint32_t address, uint32_t bufferSize, uint8_t *buffer)
{
    if ((address == SECUREBOOT_PROTECTED_FLAG_ADDRESS) && (bufferSize == SECUREBOOT_PROTECTED_FLAG_SIZE))
    {
        buffer[0] = sbGetProtectedFlag();
        return true;
    }
    else if ((address == SECUREBOOT_DEVICE_INFO_ADDRESS) && (bufferSize == SECUREBOOT_DEVICE_INFO_SIZE))
    {
        return sbGetDeviceInfo(buffer);
    }
    else if ((address == SECUREBOOT_DEVICE_SIGNATURE_ADDRESS) && (bufferSize == SECUREBOOT_DEVICE_SIGNATURE_SIZE))
    {
        return sbGetDeviceInfoSignature(buffer);
    }
    else if ((address == SECUREBOOT_FIRMWARE_SIGNATURE_ADDRESS) && (bufferSize == SECUREBOOT_FIRMWARE_SIGNATURE_SIZE))
    {
        return sbGetFirmwareSignature(buffer);
    }
    else
    {
        return false;
    }
}

bool sbProtect()
{
    secureboot_t sb;
    memcpy(&sb, &secureboot, sizeof(secureboot_t));

    sb.protectedFlag = FLAG_ENABLED;
    sb.enabledFlag = FLAG_ENABLED;

    // Generate a key pair
    if (!cryptoInitECC())
    {
        return false;
    }

    if (!cryptoGenerateECCKeyPair(sb.devicePrivateKey, sizeof(sb.devicePrivateKey), sb.devInfo.devicePublicKey, sizeof(sb.devInfo.devicePublicKey)))
    {
        cryptoCleanECC();
        return false;
    }

    cryptoCleanECC();

    flWrite(sb.bytes, sizeof(secureboot_t), (uint32_t)&secureboot);

    blRequestReboot();
    return true;
}

bool sbUnprotect()
{
    flErasePage((uint32_t)&secureboot);
    blRequestReboot();
    return true;
}

bool sbWrite(uint32_t address, uint32_t bufferSize, uint8_t *buffer)
{
    if ((address == SECUREBOOT_PROTECTED_FLAG_ADDRESS) && (bufferSize == SECUREBOOT_PROTECTED_FLAG_SIZE))
    {
        return sbSetProtectedFlag(buffer[0]);
    }
    else if ((address == SECUREBOOT_DEVICE_INFO_ADDRESS) && (bufferSize == SECUREBOOT_DEVICE_INFO_WRITE_SIZE))
    {
        return sbSetDeviceInfo((deviceInfo_t *)buffer);
    }
    else if ((address == SECUREBOOT_DEVICE_SIGNATURE_ADDRESS) && (bufferSize == SECUREBOOT_DEVICE_SIGNATURE_SIZE))
    {
        return sbSetDeviceInfoSignature(buffer);
    }
    else if ((address == SECUREBOOT_FIRMWARE_SIGNATURE_ADDRESS) && (bufferSize == SECUREBOOT_FIRMWARE_SIGNATURE_SIZE))
    {
        return sbSetFirmwareSignature(buffer);
    }
    else
    {
        return false;
    }
}
