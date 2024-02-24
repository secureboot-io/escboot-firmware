/*
 * Library: escboot
 * File:    bootloader/include/io/gpio.h
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
 * The headerfile include/io/gpio.h contains the definitions and prototypes
 * for routines that can be used to interact with the GPIO pins.
 */

#ifndef DEF_IO_GPIO_H
#define DEF_IO_GPIO_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the pin as an input with a pull-up
 * 
*/
void pinSetInputPullUp();

/**
 * @brief Set the pin as an input with a pull-down
 * 
*/
void pinSetInputPullDown();

/**
 * @brief Set the pin as an input with no pull
 * 
*/
void pinSetInputPullNone();

/**
 * @brief Set the pin as an output
 * 
*/
void pinSetOutput();

/**
 * @brief initialize the pin
 * 
*/
void pinInit();

/**
 * @brief Write to the pin
 * 
 * @param set pin state
*/
void pinWrite(bool set);

/**
 * @brief Read from the pin
 * 
 * @return bool pin state
*/
bool pinRead();

/**
 * @brief Check if the pin has a signal
 * 
 * @return bool true if the pin has a signal
 * @return bool false if the pin does not have a signal
*/
bool pinHasSignal();

#ifdef __cplusplus
} // extern "C"
#endif

#endif //DEF_IO_GPIO_H