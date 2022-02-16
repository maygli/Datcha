/*
 * Copyright (c) 2022 Maygli (mmaygli@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include <esp_err.h>
#include <cJSON.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t JSU_ConverBool(cJSON* theJSON, bool* theRes);
esp_err_t JSU_ConverInt(cJSON* theJSON, int* theRes);
esp_err_t JSU_ConverString(cJSON* theJSON, char* theBuffer, size_t theStringMaxSize);
esp_err_t JSU_ConvertIP(cJSON* theJSON, uint8_t* theRes);

#ifdef __cplusplus
extern }
#endif