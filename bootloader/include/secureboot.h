/*
 * Library: am32secureboot
 * File:    bootloader/include/secureboot.h
 * Author:  Sidhant Goel
 *
 * This file is licensed under the MIT License as stated below
 *
 * Copyright (c) 2024 Sidhant Goel 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Description
 * -----------
 * The headerfile include/secureboot.h contains the definitions and prototypes
 * for routines that can be used to interact with the secure boot module.
 */
#ifndef DEF_SECUREBOOT_H
#define DEF_SECUREBOOT_H

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

typedef struct __attribute__((packed))
{
    uint8_t manufacturerId[32];
    uint8_t deviceId[32];
    uint8_t manufacturerPublicKey[64];
    uint8_t devicePublicKey[64];

} deviceInfo_t;

typedef union __attribute__((packed))
{
    uint8_t bytes[1024];
    uint16_t words[512];
    uint32_t dwords[256];
    struct
    {
        uint8_t protectedFlag;
        uint8_t enabledFlag;
        deviceInfo_t devInfo;
        uint8_t deviceSignature[64];
        uint8_t devicePrivateKey[32];
        uint8_t firmwareSignature[64];
    };
} secureboot_t;

#define SECUREBOOT_PROTECTED_FLAG_ADDRESS       0x0800F000
#define SECUREBOOT_PROTECTED_FLAG_SIZE          0x00000001
#define SECUREBOOT_DEVICE_INFO_ADDRESS          0x0800F001
#define SECUREBOOT_DEVICE_INFO_SIZE             0x000000C0
#define SECUREBOOT_DEVICE_INFO_WRITE_SIZE       0x00000080
#define SECUREBOOT_DEVICE_SIGNATURE_ADDRESS     0x0800F0C1
#define SECUREBOOT_DEVICE_SIGNATURE_SIZE        0x00000040
#define SECUREBOOT_FIRMWARE_SIGNATURE_ADDRESS   0x0800F101
#define SECUREBOOT_FIRMWARE_SIGNATURE_SIZE      0x00000040


_Static_assert(sizeof(deviceInfo_t) == SECUREBOOT_DEVICE_INFO_SIZE, "Device info size mismatch");
_Static_assert(sizeof(uint8_t) == SECUREBOOT_PROTECTED_FLAG_SIZE, "Protected flag size mismatch");
_Static_assert(sizeof(uint8_t[64]) == SECUREBOOT_DEVICE_SIGNATURE_SIZE, "Device signature size mismatch");
_Static_assert(sizeof(uint8_t[64]) == SECUREBOOT_FIRMWARE_SIGNATURE_SIZE, "Firmware signature size mismatch");

_Static_assert(SECUREBOOT_PROTECTED_FLAG_ADDRESS + SECUREBOOT_PROTECTED_FLAG_SIZE == SECUREBOOT_DEVICE_INFO_ADDRESS,
    "SECUREBOOT_DEVICE_INFO_ADDRESS overlaps SECUREBOOT_PROTECTED_FLAG_ADDRESS");
_Static_assert(SECUREBOOT_DEVICE_INFO_ADDRESS + SECUREBOOT_DEVICE_INFO_SIZE == SECUREBOOT_DEVICE_SIGNATURE_ADDRESS,
    "SECUREBOOT_DEVICE_SIGNATURE_ADDRESS overlaps SECUREBOOT_DEVICE_INFO_ADDRESS");
_Static_assert(SECUREBOOT_DEVICE_SIGNATURE_ADDRESS + SECUREBOOT_DEVICE_SIGNATURE_SIZE == SECUREBOOT_FIRMWARE_SIGNATURE_ADDRESS,
    "SECUREBOOT_FIRMWARE_SIGNATURE_ADDRESS overlaps SECUREBOOT_DEVICE_SIGNATURE_ADDRESS");

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize secure boot module
 * 
 * @return true if successful
 * @return false if failed
 *
 * @note This function should be called before any other secure boot functions
 * 
 */
bool sbInit();

/**
 * @brief Writes to secure boot area, this is used to run secureboot specific commands
 *
 * @param address Address to write to
 * @param bufferSize Size of the buffer
 * @param buffer Buffer to write
 * @return true if successful
 * @return false if failed
 */
bool sbWrite(uint32_t address, uint32_t bufferSize, uint8_t *buffer);

/**
 * @brief Reads from secure boot area
 *
 * @param address Address to read from
 * @param bufferSize Size of the buffer
 * @param buffer Buffer to read into
 * @return true if successful
 * @return false if failed
 */
bool sbRead(uint32_t address, uint32_t bufferSize, uint8_t *buffer);

/**
 * @brief Enables protection
 *
 * @return true if successful
 * @return false if failed
 */
bool sbProtect();

/**
 * @brief Disables protection
 *
 * @return true if successful
 * @return false if failed
 */
bool sbUnprotect();

/**
 * @brief Checks if protection is enabled
 *
 * @return true if protected
 * @return false if not protected
 */
bool sbIsProtected();

/**
 * @brief Gets device info signature
*/
bool sbGetDeviceInfoSignature(uint8_t *buffer);

/**
 * @brief Sets device info signature

*/
bool sbSetDeviceInfoSignature(uint8_t *buffer);

/**
 * @brief Checks if secure boot is enabled, signature is valid
*/
bool sbOk();

#ifdef __cplusplus
}
#endif

#endif //DEF_SECUREBOOT_H