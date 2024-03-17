/**
 * Library: escboot
 * File:    bootloader/include/target_bootloader.h
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
 * The headerfile include/target_bootloader.h contains the definitions and prototypes
 * for routines that can be used to interact with the bootloader.
 */

#ifndef DEF_TARGET_BOOTLOADER_H
#define DEF_TARGET_BOOTLOADER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Base address of the flash memory
*/
#define FLASH_BASE_ADDRESS                  0x08000000

/**
 * Offset at wich the application starts
*/
#define FLASH_APPLICATION_START_OFFSET      0x00001000

/**
 * Offset at which the application ends
*/
#define FLASH_APPLICATION_END_OFFSET        0x00009000

/**
 * Offset at which the application data for signarure calculation starts
*/
#define FLASH_APPLICATION_DATA_OFFSET       0x00001000

/**
 * Offset at which the application data for signature calculation ends
*/
#define FLASH_APPLICATION_DATA_END_OFFSET   0x00007C00

/**
 * Offset at which the signature is stored
*/
#define FLASH_SIGNATURE_OFFSET              0x00008000

/**
 * Size of the signature
*/
#define FLASH_SIGNATURE_SIZE                0x00000020

/**
 * Offset at which the firmware version is stored
*/
#define FLASH_FIRMWARE_VERSION_OFFSET       0x00007BE0

/**
 * Size of the firmware version
*/
#define FLASH_FIRMWARE_VERSION_SIZE         0x00000020

/**
 * Address of the application start
*/
#define FLASH_APPLICATION_START_ADDRESS     (FLASH_BASE_ADDRESS + FLASH_APPLICATION_START_OFFSET)

/**
 * Address of the application end
*/
#define FLASH_APPLICATION_END_ADDRESS       (FLASH_BASE_ADDRESS + FLASH_APPLICATION_END_OFFSET)

/**
 * Address of the application data start
*/
#define FLASH_APPLICATION_DATA_ADDRESS      (FLASH_BASE_ADDRESS + FLASH_APPLICATION_DATA_OFFSET)

/**
 * Address of the application data end
*/
#define FLASH_APPLICATION_DATA_END_ADDRESS  (FLASH_BASE_ADDRESS + FLASH_APPLICATION_DATA_END_OFFSET)

/**
 * Address of the signature
*/
#define FLASH_SIGNATURE_ADDRESS             (FLASH_BASE_ADDRESS + FLASH_SIGNATURE_OFFSET)

/**
 * Address of the firmware version
*/
#define FLASH_FIRMWARE_VERSION_ADDRESS      (FLASH_BASE_ADDRESS + FLASH_FIRMWARE_VERSION_OFFSET)


/**
 * @brief reboot
*/
void blTargetReboot();

/**
 * @brief goto application
*/
bool blTargetGotoApplication();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DEF_TARGET_BOOTLOADER_H