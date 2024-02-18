/*
 * Library: am32secureboot
 * File:    bootloader/include/crypto/ecc.h
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
 * The headerfile include/checksum.h contains the definitions and prototypes
 * for routines that can be used to calculate several kinds of checksums.
 */
#ifndef DEF_BOOTLOADER_INCLUDE_CRYPTO_ECC_H
#define DEF_BOOTLOADER_INCLUDE_CRYPTO_ECC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the ECC module
*/
bool crypto_ecc_startup();

/**
 * @brief Generate a new ECC key pair
 * 
 * @param privateKey The buffer to store the private key
 * @param privateKeyLength The length of the private key buffer
 * @param publicKey The buffer to store the public key
 * @param publicKeyLength The length of the public key buffer
 * @return true if the key pair was generated successfully
 * @return false if the key pair could not be generated
 */
bool crypto_ecc_generate(uint8_t *privateKey, size_t privateKeyLength, uint8_t *publicKey, size_t publicKeyLength);

/**
 * @brief Clean up the ECC module
*/
void crypto_ecc_cleanup();

#ifdef __cplusplus
}
#endif


#endif // DEF_BOOTLOADER_INCLUDE_CRYPTO_ECC_H