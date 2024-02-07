/*
 * Library: libcrc
 * File:    include/checksum.h
 * Author:  Sidhant Goel
 *
 * This file is licensed under the MIT License as stated below
 *
 * Copyright (c) 1999-2018 Sidhant Goel
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

#ifndef DEF_SERIALWIRE_H
#define DEF_SERIALWIRE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define START_BIT_TIMEOUT_MS 2

#define BIT_TIME (52)       // 52uS
#define BIT_TIME_HALVE      (BIT_TIME >> 1) // 26uS
#define BIT_TIME_3_4        (BIT_TIME_HALVE + (BIT_TIME_HALVE >> 1))   // 39uS
#define START_BIT_TIME      (BIT_TIME_3_4)

bool readChar(uint8_t *buffer);
bool writeChar(uint8_t buffer);
bool readBuffer(uint8_t *buffer, size_t bufferLen, size_t *outLen);
bool writeBuffer(uint8_t *buffer, size_t bufferLen);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //DEF_SERIALWIRE_H