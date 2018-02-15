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

#include <string.h>
#ifndef __APPLE__
#  include <bsd/string.h>   // libbsd-dev
#endif
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

//#include "freertos/FreeRTOS.h"
//#include "freertos/semphr.h"
//#include "esp_log.h"
#include "datastore.h"
//#include "constants.h"
//#include "avr_support.h"

#define TAG "datastore"

#define ESP_LOGE(...)
#define ESP_LOGW(...)
#define ESP_LOGI(...)
#define ESP_LOGD(...)
#define ESP_LOG_BUFFER_HEXDUMP(...)

#define SemaphoreHandle_t void *
#define xSemaphoreCreateMutex(...) (NULL)
#define xSemaphoreTake(...)
#define xSemaphoreGive(...)

static void _get_handler(uint8_t * src, uint8_t * dest, size_t len)
{
    memcpy(dest, src, len);
}

static void _set_handler(uint8_t * src, uint8_t * dest, size_t len)
{
    memcpy(dest, src, len);
}

// String instances are handled differently, using a linked list of variable
// size strings. All other types are fixed size for fast lookup of instances.

struct callback_entry_t
{
    struct callback_entry_t * next;
    set_callback func;
    void * context;
};
typedef struct callback_entry_t callback_entry_t;

typedef struct
{
    datastore_resource_id_t id;   // not necessary? Keep as a check
    const char * name;
    datastore_type_t type;
    datastore_instance_id_t num_instances;
    void * data;   // pointer to first byte of first instance
    size_t size;   // per instance size
    bool managed;  // data allocation is managed by API
    callback_entry_t * callbacks;

} index_row_t;

typedef struct
{
    // TODO: replace with dynamic array of pointers to index rows (rather than dynamic full array)
    index_row_t * index_rows;
    size_t index_size;
} private_t;

#define _error(f, ...) do { fprintf(stdout /*stderr*/, f"\n", ##__VA_ARGS__); } while (0)
#define _debug(f, ...)
//#define _debug(f, ...) do { fprintf(stdout, f"\n", ##__VA_ARGS__); } while (0)

datastore_t * datastore_create(void)
{
    datastore_t * datastore = NULL;
    private_t * private = malloc(sizeof(*private));
    if (private != NULL)
    {
        memset(private, 0, sizeof(*private));
        _debug("malloc private %p", private);
        private->index_rows = NULL;
        private->index_size = 0;

        datastore = malloc(sizeof(*datastore));
        if (datastore)
        {
            _debug("malloc datastore %p", datastore);
            memset(datastore, 0, sizeof(*datastore));
            datastore->private_data = private;
        }
        else
        {
            _error("malloc failed");
            free(private);
        }
    }
    else
    {
        _error("malloc failed");
    }

    return datastore;
}

void datastore_free(datastore_t ** datastore)
{
    if (datastore != NULL && (*datastore != NULL))
    {
        private_t * private = (private_t *)(*datastore)->private_data;
        if (private != NULL)
        {
            index_row_t * index_rows = private->index_rows;
            for (size_t i = 0; i < private->index_size / sizeof(index_row_t); ++i)
            {
                // rely on null initialisation of index rows
                if (private->index_rows[i].managed)
                {
                    free(private->index_rows[i].data);
                }
                private->index_rows[i].data = NULL;

                if (private->index_rows[i].callbacks != NULL)
                {
                    callback_entry_t * entry = private->index_rows[i].callbacks;
                    while (entry != NULL)
                    {
                        callback_entry_t * next = entry->next;
                        free(entry);
                        entry = next;
                    }
                }
            }
            free(private->index_rows);
            private->index_rows = NULL;
            private->index_size = 0;
        }

        _debug("free private %p", private);
        free(private);
        (*datastore)->private_data = NULL;
        free(*datastore);
        *datastore = NULL;
    }
    else
    {
        _error("invalid pointer");
    }
}

// must be in same order as datastore_type_t!
uint8_t TYPE_SIZES[DATASTORE_TYPE_LAST] = {
    sizeof(bool),
    sizeof(uint8_t),
    sizeof(uint32_t),
    sizeof(int8_t),
    sizeof(int32_t),
    sizeof(float),
    sizeof(double),
    0,    // string is handled differently
};

// TODO: check strings are pooled

static datastore_status_t _add_resource(const datastore_t * datastore, datastore_resource_id_t resource_id, datastore_type_t type, uint32_t num_instances, void * data, size_t size, bool managed)
{
    datastore_status_t err = DATASTORE_STATUS_UNKNOWN;
    if (datastore != NULL)
    {
        private_t * private = (private_t *)datastore->private_data;
        if (private != NULL)
        {
            if (resource_id >= 0)
            {
                if (type >= 0 && type < DATASTORE_TYPE_LAST)
                {
                    if (num_instances > 0)
                    {
                        if (data != NULL)
                        {
                            size_t rows_in_index = private->index_size / sizeof(index_row_t);
                            if (resource_id >= rows_in_index)
                            {
                                // must extend index
                                size_t new_rows_in_index = resource_id + 1;
                                size_t old_size = private->index_size;
                                size_t new_size = new_rows_in_index * sizeof(index_row_t);
                                _debug("extend from %lu to %lu rows, 0x%lx to 0x%lx bytes", rows_in_index, new_rows_in_index, old_size, new_size);

                                index_row_t * tmp = realloc(private->index_rows, new_size);
                                if (tmp != NULL)
                                {
                                    // zero new memory
                                    if (private->index_rows != NULL)
                                    {
                                        memset(tmp + rows_in_index, 0, new_size - old_size);
                                    }
                                    else
                                    {
                                        memset(tmp, 0, new_size);
                                    }

                                    private->index_rows = tmp;
                                    private->index_size = new_size;
                                }
                                else
                                {
                                    _error("realloc failed");
                                    err = DATASTORE_STATUS_ERROR_OUT_OF_MEMORY;
                                    free(data);
                                    goto out;
                                }
                            }

                            // TODO: check for overwrite
                            _debug("register id %d, data %p", resource_id, data);
                            private->index_rows[resource_id].id = resource_id;
                            private->index_rows[resource_id].data = data;
                            private->index_rows[resource_id].name = "TODO";
                            private->index_rows[resource_id].num_instances = num_instances;
                            private->index_rows[resource_id].size = size;
                            private->index_rows[resource_id].type = type;
                            private->index_rows[resource_id].managed = managed;
                            private->index_rows[resource_id].callbacks = NULL;

                            err = DATASTORE_STATUS_OK;
                        }
                        else
                        {
                            err = DATASTORE_STATUS_ERROR_NULL_POINTER;
                            _error("data is NULL");
                        }
                    }
                    else
                    {
                        err = DATASTORE_STATUS_ERROR_INVALID_INSTANCE;
                        _error("%d instances is invalid", num_instances);
                    }
                }
                else
                {
                    err = DATASTORE_STATUS_ERROR_INVALID_TYPE;
                    _error("resource type %d is invalid", type);
                }
            }
            else
            {
                err = DATASTORE_STATUS_ERROR_INVALID_ID;
                _error("resource ID %d is invalid", resource_id);
            }
        }
        else
        {
            _error("private is NULL");
            err = DATASTORE_STATUS_ERROR_NULL_POINTER;
        }
    }
    else
    {
        err = DATASTORE_STATUS_ERROR_NULL_POINTER;
        _error("datastore is NULL");
    }
out:
    return err;
}

datastore_resource_t datastore_create_resource(datastore_type_t type, uint32_t num_instances)
{
    datastore_resource_t resource = { 0 };
    if (type >= 0 && type < DATASTORE_TYPE_LAST && type != DATASTORE_TYPE_STRING)
    {
        size_t size = TYPE_SIZES[type];
        void * data = malloc(size * num_instances);
        if (data != NULL)
        {
            memset(data, 0, size * num_instances);
            resource.data = data;
            resource.size = size;  // per instance size
            resource.num_instances = num_instances;
            resource.type = type;
            resource._managed = true;
        }
        else
        {
            _error("malloc returned NULL");
        }
    }
    else
    {
        _error("resource type %d is invalid", type);
    }
    return resource;
}

datastore_resource_t datastore_create_string_resource(size_t length, uint32_t num_instances)
{
    datastore_resource_t resource = { 0 };
    size_t size = length;
    void * data = malloc(size * num_instances);
    if (data != NULL)
    {
        memset(data, 0, size * num_instances);
        resource.data = data;
        resource.size = size;  // per instance size
        resource.num_instances = num_instances;
        resource.type = DATASTORE_TYPE_STRING;
        resource._managed = true;
    }
    else
    {
        _error("malloc returned NULL");
    }
    return resource;
}

datastore_status_t datastore_add_resource(const datastore_t * datastore, datastore_resource_id_t resource_id, const datastore_resource_t resource)
{
    return _add_resource(datastore, resource_id, resource.type, resource.num_instances, resource.data, resource.size, resource._managed);
}

datastore_status_t datastore_add_fixed_length_resource(const datastore_t * datastore, datastore_resource_id_t resource_id, datastore_type_t type, uint32_t num_instances)
{
    datastore_status_t err = DATASTORE_STATUS_UNKNOWN;
    size_t size = TYPE_SIZES[type];
    if (type != DATASTORE_TYPE_STRING)
    {
        void * data = malloc(size * num_instances);
        if (data != NULL)
        {
            memset(data, 0, size * num_instances);
            err = _add_resource(datastore, resource_id, type, num_instances, data, size, true);
            if (err != DATASTORE_STATUS_OK)
            {
                free(data);
                data = NULL;
            }
        }
        else
        {
            err = DATASTORE_STATUS_ERROR_NULL_POINTER;
            _error("malloc returned NULL");
        }
    }
    else
    {
        err = DATASTORE_STATUS_ERROR_INVALID_TYPE;
        _error("resource type %d is invalid", type);
    }
    return err;
}

datastore_status_t datastore_add_string_resource(const datastore_t * datastore, datastore_resource_id_t resource_id, uint32_t num_instances, size_t length)
{
    datastore_status_t err = DATASTORE_STATUS_UNKNOWN;
    size_t size = length;
    void * data = malloc(size * num_instances);
    if (data != NULL)
    {
        memset(data, 0, size * num_instances);
        err = _add_resource(datastore, resource_id, DATASTORE_TYPE_STRING, num_instances, data, size, true);
        if (err != DATASTORE_STATUS_OK)
        {
            free(data);
            data = NULL;
        }
    }
    else
    {
        err = DATASTORE_STATUS_ERROR_NULL_POINTER;
        _error("malloc returned NULL");
    }
    return err;
}

static datastore_status_t _set_value(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, const void * value, size_t size, datastore_type_t expected_type)
{
    _debug("_set_value: id %d, instance %d, value %p, size %d, expected_type %d", id, instance, value, size, expected_type);
    datastore_status_t err = DATASTORE_STATUS_UNKNOWN;
    if (datastore != NULL)
    {
        private_t * private = (private_t *)datastore->private_data;
        if (private != NULL)
        {
            if (id >= 0 && id < private->index_size / sizeof(index_row_t))
            {
                // check type
                if (private->index_rows[id].type == expected_type)
                {
                    // check instance
                    if (instance >= 0 && instance < private->index_rows[id].num_instances)
                    {
                        if (size <= private->index_rows[id].size)
                        {
                            if (value != NULL)
                            {
                                // finally, set the value
                                uint8_t * pdest = (uint8_t *)private->index_rows[id].data + instance * private->index_rows[id].size;
                                _debug("_set_value2: id %d, instance %d, value %p, type %d, data %p, size 0x%lx, pdest %p",
                                       id, instance, value, private->index_rows[id].type, private->index_rows[id].data, private->index_rows[id].size, pdest);

                                xSemaphoreTake(private->semaphore, portMAX_DELAY);
                                _set_handler((uint8_t *)value, pdest, private->index_rows[id].size);
                                ESP_LOG_BUFFER_HEXDUMP(TAG, pdest, private->index_rows[id].size, ESP_LOG_DEBUG);
                                xSemaphoreGive(private->semaphore);

                                // call any registered callbacks with new value
                                if (private->index_rows[id].callbacks != NULL)
                                {
                                    callback_entry_t * entry = private->index_rows[id].callbacks;
                                    while (entry != NULL)
                                    {
                                        entry->func(datastore, id, instance, entry->context);
                                        entry = entry->next;
                                    }
                                }

                                err = DATASTORE_STATUS_OK;
                            }
                            else
                            {
                                _error("_set_value2: value is NULL");
                                err = DATASTORE_STATUS_ERROR_NULL_POINTER;
                            }
                        }
                        else
                        {
                            _error("_set_value2: value size %lu exceeds allocated size %lu", size, private->index_rows[id].size);
                            err = DATASTORE_STATUS_ERROR_TOO_LARGE;
                        }
                    }
                    else
                    {
                        _error("_set_value2: instance %d is invalid", instance);
                        err = DATASTORE_STATUS_ERROR_INVALID_INSTANCE;
                    }
                }
                else
                {
                    _error("_set_value2: bad type %d (expected %d)", private->index_rows[id].type, expected_type);
                    err = DATASTORE_STATUS_ERROR_INVALID_TYPE;
                }
            }
            else
            {
                _error("_set_value2: bad id %d", id);
                err = DATASTORE_STATUS_ERROR_INVALID_ID;
            }
        }
        else
        {
            _error("_set_value2: private is NULL");
            err = DATASTORE_STATUS_ERROR_NULL_POINTER;
        }
    }
    else
    {
        _error("_set_value2: datastore is NULL");
        err = DATASTORE_STATUS_ERROR_NULL_POINTER;
    }
    return err;
}

datastore_status_t datastore_set_bool(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, bool value)
{
    return _set_value(datastore, id, instance, &value, sizeof(bool), DATASTORE_TYPE_BOOL);
}

datastore_status_t datastore_set_uint8(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, uint8_t value)
{
    return _set_value(datastore, id, instance, &value, sizeof(uint8_t), DATASTORE_TYPE_UINT8);
}

datastore_status_t datastore_set_uint32(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, uint32_t value)
{
    return _set_value(datastore, id, instance, &value, sizeof(uint32_t), DATASTORE_TYPE_UINT32);
}

datastore_status_t datastore_set_int8(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, int8_t value)
{
    return _set_value(datastore, id, instance, &value, sizeof(int8_t), DATASTORE_TYPE_INT8);
}

datastore_status_t datastore_set_int32(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, int32_t value)
{
    return _set_value(datastore, id, instance, &value, sizeof(int32_t), DATASTORE_TYPE_INT32);
}

datastore_status_t datastore_set_float(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, float value)
{
    return _set_value(datastore, id, instance, &value, sizeof(float), DATASTORE_TYPE_FLOAT);
}

datastore_status_t datastore_set_double(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, double value)
{
    return _set_value(datastore, id, instance, &value, sizeof(double), DATASTORE_TYPE_DOUBLE);
}

datastore_status_t datastore_set_string(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, const char * value)
{
    return _set_value(datastore, id, instance, value, strlen(value) + 1, DATASTORE_TYPE_STRING);
}

static datastore_status_t _get_value(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, void * value, datastore_type_t expected_type)
{
    _debug("_get_value: id %d, instance %d, value %p, expected_type %d", id, instance, value, expected_type);
    datastore_status_t err = DATASTORE_STATUS_UNKNOWN;
    if (datastore != NULL)
    {
        private_t * private = (private_t *)datastore->private_data;
        if (private != NULL)
        {
            if (id >= 0 && id < private->index_size / sizeof(index_row_t))
            {
                // check type
                if (private->index_rows[id].type == expected_type)
                {
                    // check instance
                    if (instance >= 0 && instance < private->index_rows[id].num_instances)
                    {
                        if (value)
                        {
                            // finally, get the value
                            uint8_t * psrc = (uint8_t *)private->index_rows[id].data + instance * private->index_rows[id].size;
                            _debug("_get_value2: id %d, instance %d, value %p, type %d, data %p, size 0x%lx, psrc %p",
                                   id, instance, value, private->index_rows[id].type, private->index_rows[id].data, private->index_rows[id].size, psrc);
                            ESP_LOG_BUFFER_HEXDUMP(TAG, psrc, private->index_rows[id].size, ESP_LOG_DEBUG);

                            xSemaphoreTake(private->semaphore, portMAX_DELAY);
                            _get_handler(psrc, (uint8_t *)value, private->index_rows[id].size);
                            xSemaphoreGive(private->semaphore);

                            // TODO: call any registered callbacks with new value
                            err = DATASTORE_STATUS_OK;
                        }
                        else
                        {
                            _error("_get_value2: value is NULL");
                            err = DATASTORE_STATUS_ERROR_NULL_POINTER;
                        }
                    }
                    else
                    {
                        _error("_get_value2: instance %d is invalid", instance);
                        err = DATASTORE_STATUS_ERROR_INVALID_INSTANCE;
                    }
                }
                else
                {
                    _error("_get_value2: bad type %d (expected %d)", private->index_rows[id].type, expected_type);
                    err = DATASTORE_STATUS_ERROR_INVALID_TYPE;
                }
            }
            else
            {
                _error("_get_value2: bad id %d", id);
                err = DATASTORE_STATUS_ERROR_INVALID_ID;
            }
        }
        else
        {
            _error("_get_value2: private is NULL");
            err = DATASTORE_STATUS_ERROR_NULL_POINTER;
        }
    }
    else
    {
        _error("_get_value2: datastore is NULL");
        err = DATASTORE_STATUS_ERROR_NULL_POINTER;
    }
    return err;
}

datastore_status_t datastore_get_bool(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, bool * value)
{
    return _get_value(datastore, id, instance, value, DATASTORE_TYPE_BOOL);
}

datastore_status_t datastore_get_uint8(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, uint8_t * value)
{
    return _get_value(datastore, id, instance, value, DATASTORE_TYPE_UINT8);
}

datastore_status_t datastore_get_uint32(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, uint32_t * value)
{
    return _get_value(datastore, id, instance, value, DATASTORE_TYPE_UINT32);
}

datastore_status_t datastore_get_int8(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, int8_t * value)
{
    return _get_value(datastore, id, instance, value, DATASTORE_TYPE_INT8);
}

datastore_status_t datastore_get_int32(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, int32_t * value)
{
    return _get_value(datastore, id, instance, value, DATASTORE_TYPE_INT32);
}

datastore_status_t datastore_get_float(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, float * value)
{
    return _get_value(datastore, id, instance, value, DATASTORE_TYPE_FLOAT);
}

datastore_status_t datastore_get_double(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, double * value)
{
    return _get_value(datastore, id, instance, value, DATASTORE_TYPE_DOUBLE);
}

datastore_status_t datastore_get_string(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, char * value)
{
    return _get_value(datastore, id, instance, value, DATASTORE_TYPE_STRING);
}

datastore_status_t datastore_add_set_callback(const datastore_t * datastore, datastore_resource_id_t id, set_callback callback, void * context)
{
    _debug("_get_value2: id %d, instance %d, value %p, expected_type %d", id, instance, value, expected_type);
    datastore_status_t err = DATASTORE_STATUS_UNKNOWN;
    if (datastore != NULL)
    {
        private_t * private = (private_t *)datastore->private_data;
        if (private != NULL)
        {
            if (id >= 0 && id < private->index_size / sizeof(index_row_t))
            {
                if (private->index_rows[id].callbacks == NULL)
                {
                    private->index_rows[id].callbacks = malloc(sizeof(*private->index_rows[id].callbacks));
                    private->index_rows[id].callbacks->next = NULL;
                    private->index_rows[id].callbacks->func = callback;
                    private->index_rows[id].callbacks->context = context;
                }
                else
                {
                    // find end of list
                    callback_entry_t * entry = private->index_rows[id].callbacks;
                    while (entry->next != NULL)
                    {
                        entry = entry->next;
                    }

                    entry->next = malloc(sizeof(*private->index_rows[id].callbacks));
                    entry->next->next = NULL;
                    entry->next->func = callback;
                    entry->next->context = context;
                }
                err = DATASTORE_STATUS_OK;
            }
            else
            {
                _error("_get_value2: bad id %d", id);
                err = DATASTORE_STATUS_ERROR_INVALID_ID;
            }
        }
        else
        {
            _error("_get_value2: private is NULL");
            err = DATASTORE_STATUS_ERROR_NULL_POINTER;
        }
    }
    else
    {
        _error("_get_value2: datastore is NULL");
        err = DATASTORE_STATUS_ERROR_NULL_POINTER;
    }
    return err;
}

#if 0
datastore_status_t datastore_toggle(datastore_t * store, datastore_id_t id, instance_id_t instance)
{
    ESP_LOGD(TAG":datastore_toggle", "id %d, instance %d", id, instance);
    datastore_status_t err = DATASTORE_STATUS_UNKNOWN;
    if ((err = _is_init(store)) == DATASTORE_STATUS_OK)
    {
        private_t * private = (private_t *)store->private_data;
        if (private != NULL)
        {
            if (id >= 0 && id < DATASTORE_ID_LAST)
            {
                // check type
                if (INDEX[id].type == DATASTORE_TYPE_BOOL)
                {
                    // check instance
                    if (/*instance >= 0 &&*/ instance < INDEX[id].num_instances)
                    {
                        bool value;

                        size_t instance_size = INDEX[id].size / INDEX[id].num_instances;
                        assert(instance_size * INDEX[id].num_instances == INDEX[id].size);

                        uint8_t * psrc = (uint8_t *)private + INDEX[id].offset + instance * instance_size;
                        ESP_LOGD(TAG, "_get_value: id %d, instance %d, value %p, type %d, private %p, offset 0x%x, size 0x%x, instance_size 0x%x, psrc %p",
                                 id, instance, value, INDEX[id].type, private, INDEX[id].offset, INDEX[id].size, instance_size, psrc);
                        ESP_LOG_BUFFER_HEXDUMP(TAG, psrc, instance_size, ESP_LOG_DEBUG);

                        xSemaphoreTake(private->semaphore, portMAX_DELAY);
                        _get_handler(psrc, (uint8_t *)&value, instance_size);
                        value = !value;
                        _set_handler((uint8_t *)&value, psrc, instance_size);
                        xSemaphoreGive(private->semaphore);

                        // TODO: call any registered callbacks with new value
                    }
                    else
                    {
                        ESP_LOGE(TAG, "_get_value: instance %d is invalid", instance);
                        err = DATASTORE_STATUS_ERROR_INVALID_INSTANCE;
                    }
                }
                else
                {
                    ESP_LOGE(TAG, "_get_value: bad type %d (expected bool)", INDEX[id].type);
                    err = DATASTORE_STATUS_ERROR_INVALID_TYPE;
                }
            }
            else
            {
                ESP_LOGE(TAG, "_get_value: bad id %d", id);
                err = DATASTORE_STATUS_ERROR_INVALID_ID;
            }
        }
        else
        {
            ESP_LOGE(TAG, "_get_value: private is NULL");
            err = DATASTORE_STATUS_ERROR_NULL_POINTER;
        }
    }
    else
    {
        ESP_LOGE(TAG, "_get_value: datastore is not initialised");
        err = DATASTORE_STATUS_ERROR_NOT_INITIALISED;
    }
    return err;
}

datastore_status_t datastore_increment(datastore_t * store, datastore_id_t id, instance_id_t instance)
{
    ESP_LOGD(TAG":datastore_increment", "id %d, instance %d", id, instance);
    datastore_status_t err = DATASTORE_STATUS_UNKNOWN;
    if ((err = _is_init(store)) == DATASTORE_STATUS_OK)
    {
        private_t * private = (private_t *)store->private_data;
        if (private != NULL)
        {
            if (id >= 0 && id < DATASTORE_ID_LAST)
            {
                if (/*instance >= 0 &&*/ instance < INDEX[id].num_instances)
                {
                    switch (INDEX[id].type)
                    {
                        case DATASTORE_TYPE_UINT8:
                        {
                            uint8_t value = 0;
                            _get_value(store, id, instance, &value, DATASTORE_TYPE_UINT8);
                            ++value;
                            _set_value(store, id, instance, &value, DATASTORE_TYPE_UINT8);
                            break;
                        }
                        case DATASTORE_TYPE_UINT32:
                        {
                            uint32_t value = 0;
                            _get_value(store, id, instance, &value, DATASTORE_TYPE_UINT32);
                            ++value;
                            _set_value(store, id, instance, &value, DATASTORE_TYPE_UINT32);
                            break;
                        }

                        case DATASTORE_TYPE_INT8:
                        {
                            uint8_t value = 0;
                            _get_value(store, id, instance, &value, DATASTORE_TYPE_INT8);
                            ++value;
                            _set_value(store, id, instance, &value, DATASTORE_TYPE_INT8);
                            break;
                        }
                        case DATASTORE_TYPE_INT32:
                        {
                            uint32_t value = 0;
                            _get_value(store, id, instance, &value, DATASTORE_TYPE_INT32);
                            ++value;
                            _set_value(store, id, instance, &value, DATASTORE_TYPE_INT32);
                            break;
                        }

                        default:
                            ESP_LOGE(TAG, "Cannot increment type %d", INDEX[id].type);
                            err = DATASTORE_STATUS_ERROR_INVALID_TYPE;
                            break;
                    }
                }
                else
                {
                    ESP_LOGE(TAG, "_get_value: instance %d is invalid", instance);
                    err = DATASTORE_STATUS_ERROR_INVALID_INSTANCE;
                }
            }
            else
            {
                ESP_LOGE(TAG, "_get_value: bad id %d", id);
                err = DATASTORE_STATUS_ERROR_INVALID_ID;
            }
        }
        else
        {
            ESP_LOGE(TAG, "_get_value: private is NULL");
            err = DATASTORE_STATUS_ERROR_NULL_POINTER;
        }
    }
    else
    {
        ESP_LOGE(TAG, "_get_value: datastore is not initialised");
        err = DATASTORE_STATUS_ERROR_NOT_INITIALISED;
    }
    return err;
}

datastore_status_t _to_string(const datastore_t * store, datastore_id_t id, instance_id_t instance, char * buffer, size_t len)
{
    datastore_status_t err = DATASTORE_STATUS_UNKNOWN;
    if (id >= 0 && id < DATASTORE_ID_LAST)
    {
        switch (INDEX[id].type)
        {
            case DATASTORE_TYPE_BOOL:
            {
                bool value = 0;
                datastore_get_bool(store, id, instance, &value);
                snprintf(buffer, len, "%s", value ? "true" : "false");
                err = DATASTORE_STATUS_OK;
                break;
            }
            case DATASTORE_TYPE_UINT8:
            {
                uint8_t value = 0;
                datastore_get_uint8(store, id, instance, &value);
                snprintf(buffer, len, "%u", value);
                err = DATASTORE_STATUS_OK;
                break;
            }
            case DATASTORE_TYPE_UINT32:
            {
                uint32_t value = 0;
                datastore_get_uint32(store, id, instance, &value);
                snprintf(buffer, len, "%u", value);
                err = DATASTORE_STATUS_OK;
                break;
            }
            case DATASTORE_TYPE_INT8:
            {
                int8_t value = 0;
                datastore_get_int8(store, id, instance, &value);
                snprintf(buffer, len, "%d", value);
                err = DATASTORE_STATUS_OK;
                break;
            }
            case DATASTORE_TYPE_INT32:
            {
                int32_t value = 0;
                datastore_get_int32(store, id, instance, &value);
                snprintf(buffer, len, "%d", value);
                err = DATASTORE_STATUS_OK;
                break;
            }
            case DATASTORE_TYPE_FLOAT:
            {
                float value = 0.0f;
                datastore_get_float(store, id, instance, &value);
                snprintf(buffer, len, "%f", value);
                err = DATASTORE_STATUS_OK;
                break;
            }
            case DATASTORE_TYPE_DOUBLE:
            {
                double value = 0.0;
                datastore_get_double(store, id, instance, &value);
                snprintf(buffer, len, "%lf", value);
                err = DATASTORE_STATUS_OK;
                break;
            }
            case DATASTORE_TYPE_STRING:
            {
                // hope that the supplied buffer is big enough...
                //ESP_LOGI(TAG, "len %d, INDEX[id].size %d", len, INDEX[id].size);
                assert(len >= INDEX[id].size);
                datastore_get_string(store, id, instance, buffer);
                err = DATASTORE_STATUS_OK;
                break;
            }
            default:
                ESP_LOGE(TAG, "unhandled type %d", INDEX[id].type);
                err = DATASTORE_STATUS_ERROR_INVALID_TYPE;
                break;
        }
    }
    else
    {
        ESP_LOGE(TAG, "invalid datastore ID %d", id);
        err = DATASTORE_STATUS_ERROR_INVALID_ID;
    }
    return err;
}

datastore_status_t datastore_dump(const datastore_t * store)
{
    datastore_status_t err = DATASTORE_STATUS_UNKNOWN;
    if ((err = _is_init(store)) == DATASTORE_STATUS_OK)
    {
        for (datastore_id_t id = 0; id < DATASTORE_ID_LAST; ++id)
        {
            err = DATASTORE_STATUS_OK;
            for (instance_id_t instance = 0; err == DATASTORE_STATUS_OK && instance < INDEX[id].num_instances; ++instance)
            {
                char value[256] = "";
                err = _to_string(store, id, instance, value, 256);
                ESP_LOGI(TAG, LOG_COLOR(LOG_COLOR_PURPLE)"%2d %-40s %d %s", id, INDEX[id].name, instance, value);
            }
            if (err != DATASTORE_STATUS_OK)
            {
                ESP_LOGE(TAG, "Error %d", err);
            }
        }
    }
    return err;
}
#endif // 0
