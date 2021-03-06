/*
 * MIT License
 *
 * Copyright (c) 2018 David Antliff
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
 */

#ifndef STRING_TO_H
#define STRING_TO_H

#include <stdint.h>
#include <stdbool.h>

bool string_to_bool(const char * in_str, bool * value);

bool string_to_uint8(const char * in_str, uint8_t * value);
bool string_to_uint16(const char * in_str, uint16_t * value);
bool string_to_uint32(const char * in_str, uint32_t * value);

bool string_to_int8(const char * in_str, int8_t * value);
bool string_to_int16(const char * in_str, int16_t * value);
bool string_to_int32(const char * in_str, int32_t * value);

bool string_to_float(const char * in_str, float * value);
bool string_to_double(const char * in_str, double * value);

#endif // STRING_TO_H
