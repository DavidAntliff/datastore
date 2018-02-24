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

#ifndef PLATFORM_ESP32_H
#define PLATFORM_ESP32_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TAG "datastore"

#define platform_error(...)    ESP_LOGE(TAG, __VA_ARGS__)
#define platform_warning(...)  ESP_LOGW(TAG, __VA_ARGS__)
#define platform_info(...)     ESP_LOGI(TAG, LOG_COLOR(LOG_COLOR_PURPLE) __VA_ARGS__)
#define platform_debug(...)    ESP_LOGD(TAG, __VA_ARGS__)
#define platform_hexdump(P, S) ESP_LOG_BUFFER_HEXDUMP(TAG, P, S, ESP_LOG_DEBUG)

typedef SemaphoreHandle_t platform_semaphore_t;
#define platform_semaphore_create()   xSemaphoreCreateMutex()
#define platform_semaphore_delete(S)  vSemaphoreDelete(S)
#define platform_semaphore_take(S)    xSemaphoreTake(S, portMAX_DELAY)
#define platform_semaphore_give(S)    xSemaphoreGive(S)

#ifdef __cplusplus
}
#endif

#endif // PLATFORM_POSIX_H

