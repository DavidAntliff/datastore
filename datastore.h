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

#ifndef DATASTORE_H
#define DATASTORE_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int datastore_id_t;

// TODO: rename to DATASTORE_STATUS_...
typedef enum
{
    DATASTORE_STATUS_UNKNOWN = -1,
    DATASTORE_STATUS_OK = 0,
	DATASTORE_STATUS_ERROR_OUT_OF_MEMORY,    // a memory allocation failed
    DATASTORE_STATUS_ERROR_NULL_POINTER,     // a parameter or variable is NULL
    DATASTORE_STATUS_ERROR_INVALID_TYPE,     // a type is incorrect or not handled
    DATASTORE_STATUS_ERROR_INVALID_ID,       // a resource ID is invalid
    DATASTORE_STATUS_ERROR_INVALID_INSTANCE, // an instance, or number of instances is invalid
    DATASTORE_STATUS_ERROR_TOO_LARGE,        // data is too large for allocated space
} datastore_status_t;

typedef enum
{
    DATASTORE_TYPE_INVALID = -1,
    DATASTORE_TYPE_BOOL,
    DATASTORE_TYPE_UINT8,
    DATASTORE_TYPE_UINT32,
    DATASTORE_TYPE_INT8,
    DATASTORE_TYPE_INT32,
    DATASTORE_TYPE_FLOAT,
    DATASTORE_TYPE_DOUBLE,
    DATASTORE_TYPE_STRING,
    DATASTORE_TYPE_LAST,
} datastore_type_t;

typedef int32_t datastore_resource_id_t;
typedef int32_t datastore_instance_id_t;

typedef struct
{
	void * private_data;
} datastore_t;

datastore_t * datastore_create(void);
void datastore_free(datastore_t ** datastore);

typedef void (*set_callback)(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, void * context);

typedef struct
{
    void * data;
    size_t size;
    datastore_type_t type;
    uint32_t num_instances;
    bool _managed;   // indicates memory is managed by this resource and will be freed along with it
} datastore_resource_t;

datastore_resource_t datastore_create_resource(datastore_type_t type, uint32_t num_instances);
datastore_resource_t datastore_create_string_resource(size_t length, uint32_t num_instances);

datastore_status_t datastore_add_resource(const datastore_t * datastore, datastore_resource_id_t resource_id, const datastore_resource_t resource);

// TODO: consider deprecating these
datastore_status_t datastore_add_fixed_length_resource(const datastore_t * datastore, datastore_resource_id_t resource_id, datastore_type_t type, uint32_t num_instances);
datastore_status_t datastore_add_string_resource(const datastore_t * datastore, datastore_resource_id_t resource_id, uint32_t num_instances, size_t length);

datastore_status_t datastore_set_bool(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, bool value);
datastore_status_t datastore_set_uint8(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, uint8_t value);
datastore_status_t datastore_set_uint32(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, uint32_t value);
datastore_status_t datastore_set_int8(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, int8_t value);
datastore_status_t datastore_set_int32(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, int32_t value);
datastore_status_t datastore_set_float(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, float value);
datastore_status_t datastore_set_double(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, double value);
datastore_status_t datastore_set_string(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, const char * value);

datastore_status_t datastore_get_bool(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, bool * value);
datastore_status_t datastore_get_uint8(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, uint8_t * value);
datastore_status_t datastore_get_uint32(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, uint32_t * value);
datastore_status_t datastore_get_int8(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, int8_t * value);
datastore_status_t datastore_get_int32(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, int32_t * value);
datastore_status_t datastore_get_float(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, float * value);
datastore_status_t datastore_get_double(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, double * value);
datastore_status_t datastore_get_string(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, char * value);

datastore_status_t datastore_add_set_callback(const datastore_t * datastore, datastore_resource_id_t id, set_callback callback, void * context);

//TODO
//datastore_status_t datastore_toggle(datastore_t * store, datastore_id_t id, instance_id_t instance);
//datastore_status_t datastore_increment(datastore_t * store, datastore_id_t id, instance_id_t instance);
//datastore_status_t datastore_dump(const datastore_t * store);


#ifdef __cplusplus
}
#endif

#endif // DATASTORE_H
