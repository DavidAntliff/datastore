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

//#include "esp_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DATASTORE_INSTANCES_TEMP 5

typedef enum
{
    DATASTORE_ID_SYSTEM_VERSION = 0,
    DATASTORE_ID_SYSTEM_BUILD_DATE_TIME,
    DATASTORE_ID_SYSTEM_UPTIME,

//    DATASTORE_ID_I2C_MASTER_DEVICE_COUNT,
//    DATASTORE_ID_I2C_ERROR_COUNT,
//    DATASTORE_ID_I2C_ERROR_TIMESTAMP,
//
    DATASTORE_ID_WIFI_SSID,
    DATASTORE_ID_WIFI_PASSWORD,
    DATASTORE_ID_WIFI_STATUS,
    DATASTORE_ID_WIFI_TIMESTAMP,
    DATASTORE_ID_WIFI_RSSI,
    DATASTORE_ID_WIFI_ADDRESS,

    DATASTORE_ID_MQTT_STATUS,
    DATASTORE_ID_MQTT_TIMESTAMP,
    DATASTORE_ID_MQTT_BROKER_ADDRESS,
    DATASTORE_ID_MQTT_BROKER_PORT,
    DATASTORE_ID_MQTT_CONNECTION_COUNT,
    DATASTORE_ID_MQTT_MESSAGE_TX_COUNT,
    DATASTORE_ID_MQTT_MESSAGE_RX_COUNT,

    DATASTORE_ID_TEMP_VALUE,
    DATASTORE_ID_TEMP_TIMESTAMP,
    DATASTORE_ID_TEMP_LABEL,
    DATASTORE_ID_TEMP_ASSIGNMENT,

    DATASTORE_ID_LIGHT_I2C_ADDRESS,
    DATASTORE_ID_LIGHT_DETECTED,
    DATASTORE_ID_LIGHT_FULL,
    DATASTORE_ID_LIGHT_VISIBLE,
    DATASTORE_ID_LIGHT_INFRARED,
    DATASTORE_ID_LIGHT_ILLUMINANCE,
    DATASTORE_ID_LIGHT_TIMESTAMP,

    DATASTORE_ID_FLOW_FREQUENCY,
    DATASTORE_ID_FLOW_RATE,

    DATASTORE_ID_POWER_VALUE,
    DATASTORE_ID_POWER_TIMESTAMP,

    DATASTORE_ID_SWITCHES_CP_MODE_VALUE,
    //DATASTORE_ID_SWITCHES_CP_MODE_COUNT,
    DATASTORE_ID_SWITCHES_CP_MAN_VALUE,
    //DATASTORE_ID_SWITCHES_CP_MAN_COUNT,
    DATASTORE_ID_SWITCHES_PP_MODE_VALUE,
    //DATASTORE_ID_SWITCHES_PP_MODE_COUNT,
    DATASTORE_ID_SWITCHES_PP_MAN_VALUE,
    //DATASTORE_ID_SWITCHES_PP_MAN_COUNT,
    DATASTORE_ID_SWITCHES_TIMESTAMP,

    DATASTORE_ID_PUMPS_CP_STATE,
    DATASTORE_ID_PUMPS_PP_STATE,

    DATASTORE_ID_LAST,
} datastore_id_t;

// string lengths
#define DATASTORE_LEN_VERSION          4
#define DATASTORE_LEN_BUILD_DATE_TIME 16
#define DATASTORE_LEN_TEMP_LABEL       8
#define DATASTORE_LEN_WIFI_SSID        64 //(sizeof(((wifi_sta_config_t *)0)->ssid))
#define DATASTORE_LEN_WIFI_PASSWORD    64 //(sizeof(((wifi_sta_config_t *)0)->password))
#define DATASTORE_LEN_MQTT_BROKER_ADDRESS 64

// TODO: rename to DATASTORE_STATUS_...
typedef enum
{
    DATASTORE_ERROR_UNKNOWN = -1,
    DATASTORE_OK = 0,
	DATASTORE_ERROR_OUT_OF_MEMORY,    // a memory allocation failed
    DATASTORE_ERROR_NULL_POINTER,     // a parameter or variable is NULL
    DATASTORE_ERROR_NOT_INITIALISED,  // the datastore is not initialised  // TODO REMOVE with V2
    DATASTORE_ERROR_INVALID_TYPE,     // a type is incorrect or not handled
    DATASTORE_ERROR_INVALID_ID,       // a resource ID is invalid
    DATASTORE_ERROR_INVALID_INSTANCE, // an instance, or number of instances is invalid
} datastore_error_t;

typedef enum
{
    DATASTORE_WIFI_STATUS_DISCONNECTED = 0,
    DATASTORE_WIFI_STATUS_CONNECTED,
    DATASTORE_WIFI_STATUS_GOT_ADDRESS,
} datastore_wifi_status_t;

typedef enum
{
    DATASTORE_MQTT_STATUS_DISCONNECTED = 0,
    DATASTORE_MQTT_STATUS_CONNECTING,
    DATASTORE_MQTT_STATUS_CONNECTED,
} datastore_mqtt_status_t;

typedef enum
{
    DATASTORE_TEMP_ASSIGNMENT_T1 = 0,
    DATASTORE_TEMP_ASSIGNMENT_T2,
    DATASTORE_TEMP_ASSIGNMENT_T3,
    DATASTORE_TEMP_ASSIGNMENT_T4,
    DATASTORE_TEMP_ASSIGNMENT_T5,
} datastore_temp_assignment_t;

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

typedef struct
{
    void * private_data;
} datastore_t;

typedef uint8_t instance_id_t;
typedef uint32_t datastore2_instance_id_t;

// allocate, initialise and free datastore
datastore_t * datastore_malloc(void);
void datastore_free(datastore_t ** store);
datastore_error_t datastore_init(datastore_t * store);

// load or save persistent data from/to NV
datastore_error_t datastore_load(void);
datastore_error_t datastore_save(void);

datastore_error_t datastore_set_bool(datastore_t * store, datastore_id_t id, instance_id_t instance, bool value);
datastore_error_t datastore_set_uint8(datastore_t * store, datastore_id_t id, instance_id_t instance, uint8_t value);
datastore_error_t datastore_set_uint32(datastore_t * store, datastore_id_t id, instance_id_t instance, uint32_t value);
datastore_error_t datastore_set_int8(datastore_t * store, datastore_id_t id, instance_id_t instance, int8_t value);
datastore_error_t datastore_set_int32(datastore_t * store, datastore_id_t id, instance_id_t instance, int32_t value);
datastore_error_t datastore_set_float(datastore_t * store, datastore_id_t id, instance_id_t instance, float value);
datastore_error_t datastore_set_double(datastore_t * store, datastore_id_t id, instance_id_t instance, double value);
datastore_error_t datastore_set_string(datastore_t * store, datastore_id_t id, instance_id_t instance, const char * value);

datastore_error_t datastore_get_bool(const datastore_t * store, datastore_id_t id, instance_id_t instance, bool * value);
datastore_error_t datastore_get_uint8(const datastore_t * store, datastore_id_t id, instance_id_t instance, uint8_t * value);
datastore_error_t datastore_get_uint32(const datastore_t * store, datastore_id_t id, instance_id_t instance, uint32_t * value);
datastore_error_t datastore_get_int8(const datastore_t * store, datastore_id_t id, instance_id_t instance, int8_t * value);
datastore_error_t datastore_get_int32(const datastore_t * store, datastore_id_t id, instance_id_t instance, int32_t * value);
datastore_error_t datastore_get_float(const datastore_t * store, datastore_id_t id, instance_id_t instance, float * value);
datastore_error_t datastore_get_double(const datastore_t * store, datastore_id_t id, instance_id_t instance, double * value);
datastore_error_t datastore_get_string(const datastore_t * store, datastore_id_t id, instance_id_t instance, char * value);

datastore_error_t datastore_toggle(datastore_t * store, datastore_id_t id, instance_id_t instance);
datastore_error_t datastore_increment(datastore_t * store, datastore_id_t id, instance_id_t instance);

datastore_error_t datastore_dump(const datastore_t * store);


// NEW API
typedef struct
{
	void * private_data;
} datastore2_t;

typedef int32_t datastore2_resource_id_t;

datastore2_t * datastore2_create(void);
void datastore2_free(datastore2_t ** datastore);

datastore_error_t datastore2_add_resource(datastore2_t * datastore, datastore2_resource_id_t resource_id, datastore_type_t type, uint32_t num_instances);

datastore_error_t datastore2_set_bool(datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, bool value);
datastore_error_t datastore2_set_uint8(datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, uint8_t value);
datastore_error_t datastore2_set_uint32(datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, uint32_t value);
datastore_error_t datastore2_set_int8(datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, int8_t value);
datastore_error_t datastore2_set_int32(datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, int32_t value);
datastore_error_t datastore2_set_float(datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, float value);
datastore_error_t datastore2_set_double(datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, double value);
datastore_error_t datastore2_set_string(datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, const char * value);

datastore_error_t datastore2_get_bool(const datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, bool * value);
datastore_error_t datastore2_get_uint8(const datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, uint8_t * value);
datastore_error_t datastore2_get_uint32(const datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, uint32_t * value);
datastore_error_t datastore2_get_int8(const datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, int8_t * value);
datastore_error_t datastore2_get_int32(const datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, int32_t * value);
datastore_error_t datastore2_get_float(const datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, float * value);
datastore_error_t datastore2_get_double(const datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, double * value);
datastore_error_t datastore2_get_string(const datastore2_t * store, datastore2_resource_id_t id, datastore2_instance_id_t instance, char * value);


#ifdef __cplusplus
}
#endif

#endif // DISPLAY_H
