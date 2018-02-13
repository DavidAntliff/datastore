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

typedef uint32_t avr_switch_mode_t;
typedef uint32_t avr_pump_state_t;
typedef uint32_t avr_switch_manual_t;

#define ESP_LOGE(...)
#define ESP_LOGW(...)
#define ESP_LOGI(...)
#define ESP_LOGD(...)
#define ESP_LOG_BUFFER_HEXDUMP(...)

#define SemaphoreHandle_t void *
#define xSemaphoreCreateMutex(...) (NULL)
#define xSemaphoreTake(...)
#define xSemaphoreGive(...)

#define VERSION "1.2.3"
#define BUILD_TIMESTAMP "2018-02-05T12:04:38"

struct _private_t
{
    SemaphoreHandle_t semaphore;

    struct data
    {
        struct
        {
            char version[DATASTORE_LEN_VERSION];
            char build_date_time[DATASTORE_LEN_BUILD_DATE_TIME];
            uint32_t uptime;
        } system;

        struct
        {
            uint8_t device_count;
            uint32_t error_count;
            uint32_t error_timestamp;
        } i2c_master;

        struct
        {
            uint8_t ssid[DATASTORE_LEN_WIFI_SSID];
            uint8_t password[DATASTORE_LEN_WIFI_PASSWORD];
            datastore_wifi_status_t status;
            uint32_t timestamp;
            int8_t rssi;
            uint32_t address;
        } wifi;

        struct
        {
            uint32_t status;
            uint32_t timestamp;
            uint8_t broker_address[DATASTORE_LEN_MQTT_BROKER_ADDRESS];
            uint32_t broker_port;
            uint32_t connection_count;
            uint32_t message_tx_count;
            uint32_t message_rx_count;
        } mqtt;

        struct
        {
            float value[DATASTORE_INSTANCES_TEMP];
            uint32_t timestamp[DATASTORE_INSTANCES_TEMP];
            char label[DATASTORE_INSTANCES_TEMP][DATASTORE_LEN_TEMP_LABEL];
            datastore_temp_assignment_t assignment[DATASTORE_INSTANCES_TEMP];
        } temp;

        struct
        {
            uint8_t i2c_address;
            bool detected;
            uint32_t full;
            uint32_t infrared;
            uint32_t visible;
            uint32_t illuminance;
            uint32_t timestamp;
        } light;

        struct
        {
            float frequency;  // Hz
            float rate;       // LPM
        } flow;

        struct
        {
            float value;   // Watts
            uint32_t timestamp;
        } power;

        struct
        {
            struct
            {
                struct
                {
                    avr_switch_mode_t value;
                    uint32_t count;
                } mode;
                struct
                {
                    avr_switch_manual_t value;
                    uint32_t count;
                } manual;
            } cp;
            struct pp
            {
                struct
                {
                    avr_switch_mode_t value;
                    uint32_t count;
                } mode;
                struct
                {
                    avr_switch_manual_t value;
                    uint32_t count;
                } manual;
            } pp;
            uint32_t timestamp;
        } switches;

        struct
        {
            struct
            {
                avr_pump_state_t state;
            } cp;
            struct
            {
                avr_pump_state_t state;
            } pp;
        } pumps;

    } data;

};
typedef struct _private_t private_t;

//typedef void (*copy_func)(uint8_t * src, uint8_t * dest, size_t len);
//
static void _get_handler(uint8_t * src, uint8_t * dest, size_t len)
{
    memcpy(dest, src, len);
}

static void _set_handler(uint8_t * src, uint8_t * dest, size_t len)
{
    memcpy(dest, src, len);
}

typedef struct
{
    datastore_id_t id;
    const char * name;
    datastore_type_t type;
    uint8_t num_instances;
    size_t offset;
    //    copy_func get_handler;
    //    copy_func set_handler;
    size_t size;
} index_t;

#define NAME(X) #X

#define INDEX_ROW(name, type, num_instances, field) { name, NAME(name), type, num_instances, offsetof(private_t, field), sizeof(((private_t *)0)->field) }

static index_t INDEX[] = {
    INDEX_ROW(DATASTORE_ID_SYSTEM_VERSION,             DATASTORE_TYPE_STRING, 1, data.system.version),
    INDEX_ROW(DATASTORE_ID_SYSTEM_BUILD_DATE_TIME,     DATASTORE_TYPE_STRING, 1, data.system.build_date_time),
    INDEX_ROW(DATASTORE_ID_SYSTEM_UPTIME,              DATASTORE_TYPE_UINT32, 1, data.system.uptime),

    INDEX_ROW(DATASTORE_ID_WIFI_SSID,                  DATASTORE_TYPE_STRING, 1, data.wifi.ssid),
    INDEX_ROW(DATASTORE_ID_WIFI_PASSWORD,              DATASTORE_TYPE_STRING, 1, data.wifi.password),
    INDEX_ROW(DATASTORE_ID_WIFI_STATUS,                DATASTORE_TYPE_UINT32, 1, data.wifi.status),
    INDEX_ROW(DATASTORE_ID_WIFI_TIMESTAMP,             DATASTORE_TYPE_UINT32, 1, data.wifi.timestamp),
    INDEX_ROW(DATASTORE_ID_WIFI_RSSI,                  DATASTORE_TYPE_INT8,   1, data.wifi.rssi),
    INDEX_ROW(DATASTORE_ID_WIFI_ADDRESS,               DATASTORE_TYPE_UINT32, 1, data.wifi.address),

    INDEX_ROW(DATASTORE_ID_MQTT_STATUS,                DATASTORE_TYPE_UINT32, 1, data.mqtt.status),
    INDEX_ROW(DATASTORE_ID_MQTT_TIMESTAMP,             DATASTORE_TYPE_UINT32, 1, data.mqtt.timestamp),
    INDEX_ROW(DATASTORE_ID_MQTT_BROKER_ADDRESS,        DATASTORE_TYPE_STRING, 1, data.mqtt.broker_address),
    INDEX_ROW(DATASTORE_ID_MQTT_BROKER_PORT,           DATASTORE_TYPE_UINT32, 1, data.mqtt.broker_port),
    INDEX_ROW(DATASTORE_ID_MQTT_CONNECTION_COUNT,      DATASTORE_TYPE_UINT32, 1, data.mqtt.connection_count),
    INDEX_ROW(DATASTORE_ID_MQTT_MESSAGE_TX_COUNT,      DATASTORE_TYPE_UINT32, 1, data.mqtt.message_tx_count),
    INDEX_ROW(DATASTORE_ID_MQTT_MESSAGE_RX_COUNT,      DATASTORE_TYPE_UINT32, 1, data.mqtt.message_rx_count),

    INDEX_ROW(DATASTORE_ID_TEMP_VALUE,                 DATASTORE_TYPE_FLOAT,  DATASTORE_INSTANCES_TEMP, data.temp.value),
    INDEX_ROW(DATASTORE_ID_TEMP_TIMESTAMP,             DATASTORE_TYPE_UINT32, DATASTORE_INSTANCES_TEMP, data.temp.timestamp),
    INDEX_ROW(DATASTORE_ID_TEMP_LABEL,                 DATASTORE_TYPE_STRING, DATASTORE_INSTANCES_TEMP, data.temp.label),
    INDEX_ROW(DATASTORE_ID_TEMP_ASSIGNMENT,            DATASTORE_TYPE_UINT8,  DATASTORE_INSTANCES_TEMP, data.temp.assignment),

    INDEX_ROW(DATASTORE_ID_LIGHT_I2C_ADDRESS,          DATASTORE_TYPE_UINT8,  1, data.light.i2c_address),
    INDEX_ROW(DATASTORE_ID_LIGHT_DETECTED,             DATASTORE_TYPE_BOOL,   1, data.light.detected),
    INDEX_ROW(DATASTORE_ID_LIGHT_FULL,                 DATASTORE_TYPE_UINT32, 1, data.light.full),
    INDEX_ROW(DATASTORE_ID_LIGHT_VISIBLE,              DATASTORE_TYPE_UINT32, 1, data.light.visible),
    INDEX_ROW(DATASTORE_ID_LIGHT_INFRARED,             DATASTORE_TYPE_UINT32, 1, data.light.infrared),
    INDEX_ROW(DATASTORE_ID_LIGHT_ILLUMINANCE,          DATASTORE_TYPE_UINT32, 1, data.light.illuminance),
    INDEX_ROW(DATASTORE_ID_LIGHT_TIMESTAMP,            DATASTORE_TYPE_UINT32, 1, data.light.timestamp),

    INDEX_ROW(DATASTORE_ID_FLOW_FREQUENCY,             DATASTORE_TYPE_FLOAT,  1, data.flow.frequency),
    INDEX_ROW(DATASTORE_ID_FLOW_RATE,                  DATASTORE_TYPE_FLOAT,  1, data.flow.rate),

    INDEX_ROW(DATASTORE_ID_POWER_VALUE,                DATASTORE_TYPE_FLOAT,  1, data.power.value),
    INDEX_ROW(DATASTORE_ID_POWER_TIMESTAMP,            DATASTORE_TYPE_UINT32, 1, data.power.timestamp),

    INDEX_ROW(DATASTORE_ID_SWITCHES_CP_MODE_VALUE,     DATASTORE_TYPE_UINT32, 1, data.switches.cp.mode.value),
    //INDEX_ROW(DATASTORE_ID_SWITCHES_CP_MODE_COUNT,     DATASTORE_TYPE_UINT32, 1, data.switches.cp.mode.count),
    INDEX_ROW(DATASTORE_ID_SWITCHES_CP_MAN_VALUE,      DATASTORE_TYPE_UINT32, 1, data.switches.cp.manual.value),
    //INDEX_ROW(DATASTORE_ID_SWITCHES_CP_MAN_COUNT,      DATASTORE_TYPE_UINT32, 1, data.switches.cp.man.count),
    INDEX_ROW(DATASTORE_ID_SWITCHES_PP_MODE_VALUE,     DATASTORE_TYPE_UINT32, 1, data.switches.pp.mode.value),
    //INDEX_ROW(DATASTORE_ID_SWITCHES_PP_MODE_COUNT,     DATASTORE_TYPE_UINT32, 1, data.switches.pp.mode.count),
    INDEX_ROW(DATASTORE_ID_SWITCHES_PP_MAN_VALUE,      DATASTORE_TYPE_UINT32, 1, data.switches.pp.manual.value),
    //INDEX_ROW(DATASTORE_ID_SWITCHES_PP_MAN_COUNT,      DATASTORE_TYPE_UINT32, 1, data.switches.pp.man.count),
    INDEX_ROW(DATASTORE_ID_SWITCHES_TIMESTAMP,         DATASTORE_TYPE_UINT32, 1, data.switches.timestamp),

    INDEX_ROW(DATASTORE_ID_PUMPS_CP_STATE,             DATASTORE_TYPE_UINT32, 1, data.pumps.cp.state),
    INDEX_ROW(DATASTORE_ID_PUMPS_PP_STATE,             DATASTORE_TYPE_UINT32, 1, data.pumps.pp.state),
};


static datastore_error_t _is_init(const datastore_t * store)
{
    datastore_error_t err = DATASTORE_ERROR_NOT_INITIALISED;
    if (store != NULL)
    {
        if (store->private_data)
        {
            // OK
            err = DATASTORE_OK;
        }
        else
        {
            ESP_LOGE(TAG, "datastore is not initialised");
        }
    }
    else
    {
        ESP_LOGE(TAG, "store is NULL");
        err = DATASTORE_ERROR_NULL_POINTER;
    }
    return err;
}

datastore_t * datastore_malloc(void)
{
    datastore_t * store = NULL;
    private_t * private = malloc(sizeof(*private));
    if (private != NULL)
    {
        memset(private, 0, sizeof(*private));
        ESP_LOGD(TAG, "malloc private %p", private);

        store = malloc(sizeof(*store));
        if (store)
        {
            ESP_LOGD(TAG, "malloc store %p", store);
            memset(store, 0, sizeof(*store));
            store->private_data = private;
        }
        else
        {
            ESP_LOGE(TAG, "malloc failed");
        }
    }
    else
    {
        ESP_LOGE(TAG, "malloc failed");
    }

    return store;
}

void datastore_free(datastore_t ** store)
{
    if (store != NULL && (*store != NULL))
    {
        ESP_LOGD(TAG, "free private %p", (*store)->private);
        free((*store)->private_data);
        free(*store);
        *store = NULL;
    }
}

datastore_error_t datastore_init(datastore_t * store)
{
    ESP_LOGD(TAG, "%s", __FUNCTION__);

    // check that the index is correct
    assert(sizeof(INDEX) / sizeof(INDEX[0]) == DATASTORE_ID_LAST);
    for (datastore_id_t id = 0; id < DATASTORE_ID_LAST; ++id)
    {
        assert(INDEX[id].id == id);
        assert(INDEX[id].type < DATASTORE_TYPE_LAST);
    }

    datastore_error_t err = DATASTORE_ERROR_UNKNOWN;
    if (store != NULL)
    {
        private_t * private = (private_t *)store->private_data;
        if (private != NULL)
        {
            private->semaphore = xSemaphoreCreateMutex();

            // populate some fields
            strlcpy(private->data.system.version, VERSION, DATASTORE_LEN_VERSION);
            strlcpy(private->data.system.build_date_time, BUILD_TIMESTAMP, DATASTORE_LEN_BUILD_DATE_TIME);

            err = DATASTORE_OK;
        }
        else
        {
            ESP_LOGE(TAG, "store->private is NULL");
            err = DATASTORE_ERROR_NULL_POINTER;
        }
    }
    else
    {
        ESP_LOGE(TAG, "store is NULL");
        err = DATASTORE_ERROR_NULL_POINTER;
    }
    return err;
}

static datastore_error_t _set_value(const datastore_t * store, datastore_id_t id, instance_id_t instance, const void * value, datastore_type_t expected_type)
{
    ESP_LOGD(TAG":_set_value", "id %d, instance %d, value %p, expected_type %d", id, instance, value, expected_type);
    datastore_error_t err = DATASTORE_ERROR_UNKNOWN;
    if ((err = _is_init(store)) == DATASTORE_OK)
    {
        private_t * private = (private_t *)store->private_data;
        if (private != NULL)
        {
            if (id >= 0 && id < DATASTORE_ID_LAST)
            {
                // check type
                if (INDEX[id].type == expected_type)
                {
                    // check instance
                    if (/*instance >= 0 &&*/ instance < INDEX[id].num_instances)
                    {
                        if (value)
                        {
                            // finally, set the value
                            size_t instance_size = INDEX[id].size / INDEX[id].num_instances;
                            assert(instance_size * INDEX[id].num_instances == INDEX[id].size);

                            uint8_t * pdest = (uint8_t *)private + INDEX[id].offset + instance * instance_size;
                            ESP_LOGD(TAG, "_set_value: id %d, instance %d, value %p, type %d, private %p, offset 0x%x, size 0x%x, instance_size 0x%x, pdest %p",
                                     id, instance, value, INDEX[id].type, private, INDEX[id].offset, INDEX[id].size, instance_size, pdest);

                            xSemaphoreTake(private->semaphore, portMAX_DELAY);
                            _set_handler((uint8_t *)value, pdest, instance_size);
                            ESP_LOG_BUFFER_HEXDUMP(TAG, pdest, instance_size, ESP_LOG_DEBUG);
                            xSemaphoreGive(private->semaphore);

                            // TODO: call any registered callbacks with new value
                        }
                        else
                        {
                            ESP_LOGE(TAG, "_set_value: value is NULL");
                            err = DATASTORE_ERROR_NULL_POINTER;
                        }
                    }
                    else
                    {
                        ESP_LOGE(TAG, "_set_value: instance %d is invalid", instance);
                        err = DATASTORE_ERROR_INVALID_INSTANCE;
                    }
                }
                else
                {
                    ESP_LOGE(TAG, "_set_value: bad type %d (expected %d)", INDEX[id].type, expected_type);
                    err = DATASTORE_ERROR_INVALID_TYPE;
                }
            }
            else
            {
                ESP_LOGE(TAG, "_set_value: bad id %d", id);
                err = DATASTORE_ERROR_INVALID_ID;
            }
        }
        else
        {
            ESP_LOGE(TAG, "_set_value: private is NULL");
            err = DATASTORE_ERROR_NULL_POINTER;
        }
    }
    else
    {
        ESP_LOGE(TAG, "_set_value: datastore is not initialised");
        err = DATASTORE_ERROR_NOT_INITIALISED;
    }
    return err;
}

datastore_error_t datastore_set_bool(datastore_t * store, datastore_id_t id, instance_id_t instance, bool value)
{
    return _set_value(store, id, instance, &value, DATASTORE_TYPE_BOOL);
}

datastore_error_t datastore_set_uint8(datastore_t * store, datastore_id_t id, instance_id_t instance, uint8_t value)
{
    return _set_value(store, id, instance, &value, DATASTORE_TYPE_UINT8);
}

datastore_error_t datastore_set_uint32(datastore_t * store, datastore_id_t id, instance_id_t instance, uint32_t value)
{
    return _set_value(store, id, instance, &value, DATASTORE_TYPE_UINT32);
}

datastore_error_t datastore_set_int8(datastore_t * store, datastore_id_t id, instance_id_t instance, int8_t value)
{
    return _set_value(store, id, instance, &value, DATASTORE_TYPE_INT8);
}

datastore_error_t datastore_set_int32(datastore_t * store, datastore_id_t id, instance_id_t instance, int32_t value)
{
    return _set_value(store, id, instance, &value, DATASTORE_TYPE_UINT32);
}

datastore_error_t datastore_set_float(datastore_t * store, datastore_id_t id, instance_id_t instance, float value)
{
    return _set_value(store, id, instance, &value, DATASTORE_TYPE_FLOAT);
}

datastore_error_t datastore_set_double(datastore_t * store, datastore_id_t id, instance_id_t instance, double value)
{
    return _set_value(store, id, instance, &value, DATASTORE_TYPE_DOUBLE);
}

datastore_error_t datastore_set_string(datastore_t * store, datastore_id_t id, instance_id_t instance, const char * value)
{
    return _set_value(store, id, instance, value, DATASTORE_TYPE_STRING);
}

static datastore_error_t _get_value(const datastore_t * store, datastore_id_t id, instance_id_t instance, void * value, datastore_type_t expected_type)
{
    ESP_LOGD(TAG":_get_value", "id %d, instance %d, value %p, expected_type %d", id, instance, value, expected_type);
    datastore_error_t err = DATASTORE_ERROR_UNKNOWN;
    if ((err = _is_init(store)) == DATASTORE_OK)
    {
        private_t * private = (private_t *)store->private_data;
        if (private != NULL)
        {
            if (id >= 0 && id < DATASTORE_ID_LAST)
            {
                // check type
                if (INDEX[id].type == expected_type)
                {
                    // check instance
                    if (/*instance >= 0 &&*/ instance < INDEX[id].num_instances)
                    {
                        if (value)
                        {
                            // finally, get the value
                            size_t instance_size = INDEX[id].size / INDEX[id].num_instances;
                            assert(instance_size * INDEX[id].num_instances == INDEX[id].size);

                            uint8_t * psrc = (uint8_t *)private + INDEX[id].offset + instance * instance_size;
                            ESP_LOGD(TAG, "_get_value: id %d, instance %d, value %p, type %d, private %p, offset 0x%x, size 0x%x, instance_size 0x%x, psrc %p",
                                     id, instance, value, INDEX[id].type, private, INDEX[id].offset, INDEX[id].size, instance_size, psrc);
                            ESP_LOG_BUFFER_HEXDUMP(TAG, psrc, instance_size, ESP_LOG_DEBUG);

                            xSemaphoreTake(private->semaphore, portMAX_DELAY);
                            _get_handler(psrc, (uint8_t *)value, instance_size);
                            xSemaphoreGive(private->semaphore);

                            // TODO: call any registered callbacks with new value
                        }
                        else
                        {
                            ESP_LOGE(TAG, "_get_value: value is NULL");
                            err = DATASTORE_ERROR_NULL_POINTER;
                        }
                    }
                    else
                    {
                        ESP_LOGE(TAG, "_get_value: instance %d is invalid", instance);
                        err = DATASTORE_ERROR_INVALID_INSTANCE;
                    }
                }
                else
                {
                    ESP_LOGE(TAG, "_get_value: bad type %d (expected %d)", INDEX[id].type, expected_type);
                    err = DATASTORE_ERROR_INVALID_TYPE;
                }
            }
            else
            {
                ESP_LOGE(TAG, "_get_value: bad id %d", id);
                err = DATASTORE_ERROR_INVALID_ID;
            }
        }
        else
        {
            ESP_LOGE(TAG, "_get_value: private is NULL");
            err = DATASTORE_ERROR_NULL_POINTER;
        }
    }
    else
    {
        ESP_LOGE(TAG, "_get_value: datastore is not initialised");
        err = DATASTORE_ERROR_NOT_INITIALISED;
    }
    return err;
}

datastore_error_t datastore_get_bool(const datastore_t * store, datastore_id_t id, instance_id_t instance, bool * value)
{
    return _get_value(store, id, instance, value, DATASTORE_TYPE_BOOL);
}

datastore_error_t datastore_get_uint8(const datastore_t * store, datastore_id_t id, instance_id_t instance, uint8_t * value)
{
    return _get_value(store, id, instance, value, DATASTORE_TYPE_UINT8);
}

datastore_error_t datastore_get_uint32(const datastore_t * store, datastore_id_t id, instance_id_t instance, uint32_t * value)
{
    return _get_value(store, id, instance, value, DATASTORE_TYPE_UINT32);
}

datastore_error_t datastore_get_int8(const datastore_t * store, datastore_id_t id, instance_id_t instance, int8_t * value)
{
    return _get_value(store, id, instance, value, DATASTORE_TYPE_INT8);
}

datastore_error_t datastore_get_int32(const datastore_t * store, datastore_id_t id, instance_id_t instance, int32_t * value)
{
    return _get_value(store, id, instance, value, DATASTORE_TYPE_INT32);
}

datastore_error_t datastore_get_float(const datastore_t * store, datastore_id_t id, instance_id_t instance, float * value)
{
    return _get_value(store, id, instance, value, DATASTORE_TYPE_FLOAT);
}

datastore_error_t datastore_get_double(const datastore_t * store, datastore_id_t id, instance_id_t instance, double * value)
{
    return _get_value(store, id, instance, value, DATASTORE_TYPE_DOUBLE);
}

datastore_error_t datastore_get_string(const datastore_t * store, datastore_id_t id, instance_id_t instance, char * value)
{
    return _get_value(store, id, instance, value, DATASTORE_TYPE_STRING);
}

datastore_error_t datastore_toggle(datastore_t * store, datastore_id_t id, instance_id_t instance)
{
    ESP_LOGD(TAG":datastore_toggle", "id %d, instance %d", id, instance);
    datastore_error_t err = DATASTORE_ERROR_UNKNOWN;
    if ((err = _is_init(store)) == DATASTORE_OK)
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
                        err = DATASTORE_ERROR_INVALID_INSTANCE;
                    }
                }
                else
                {
                    ESP_LOGE(TAG, "_get_value: bad type %d (expected bool)", INDEX[id].type);
                    err = DATASTORE_ERROR_INVALID_TYPE;
                }
            }
            else
            {
                ESP_LOGE(TAG, "_get_value: bad id %d", id);
                err = DATASTORE_ERROR_INVALID_ID;
            }
        }
        else
        {
            ESP_LOGE(TAG, "_get_value: private is NULL");
            err = DATASTORE_ERROR_NULL_POINTER;
        }
    }
    else
    {
        ESP_LOGE(TAG, "_get_value: datastore is not initialised");
        err = DATASTORE_ERROR_NOT_INITIALISED;
    }
    return err;
}

datastore_error_t datastore_increment(datastore_t * store, datastore_id_t id, instance_id_t instance)
{
    ESP_LOGD(TAG":datastore_increment", "id %d, instance %d", id, instance);
    datastore_error_t err = DATASTORE_ERROR_UNKNOWN;
    if ((err = _is_init(store)) == DATASTORE_OK)
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
                            err = DATASTORE_ERROR_INVALID_TYPE;
                            break;
                    }
                }
                else
                {
                    ESP_LOGE(TAG, "_get_value: instance %d is invalid", instance);
                    err = DATASTORE_ERROR_INVALID_INSTANCE;
                }
            }
            else
            {
                ESP_LOGE(TAG, "_get_value: bad id %d", id);
                err = DATASTORE_ERROR_INVALID_ID;
            }
        }
        else
        {
            ESP_LOGE(TAG, "_get_value: private is NULL");
            err = DATASTORE_ERROR_NULL_POINTER;
        }
    }
    else
    {
        ESP_LOGE(TAG, "_get_value: datastore is not initialised");
        err = DATASTORE_ERROR_NOT_INITIALISED;
    }
    return err;
}

datastore_error_t _to_string(const datastore_t * store, datastore_id_t id, instance_id_t instance, char * buffer, size_t len)
{
    datastore_error_t err = DATASTORE_ERROR_UNKNOWN;
    if (id >= 0 && id < DATASTORE_ID_LAST)
    {
        switch (INDEX[id].type)
        {
            case DATASTORE_TYPE_BOOL:
            {
                bool value = 0;
                datastore_get_bool(store, id, instance, &value);
                snprintf(buffer, len, "%s", value ? "true" : "false");
                err = DATASTORE_OK;
                break;
            }
            case DATASTORE_TYPE_UINT8:
            {
                uint8_t value = 0;
                datastore_get_uint8(store, id, instance, &value);
                snprintf(buffer, len, "%u", value);
                err = DATASTORE_OK;
                break;
            }
            case DATASTORE_TYPE_UINT32:
            {
                uint32_t value = 0;
                datastore_get_uint32(store, id, instance, &value);
                snprintf(buffer, len, "%u", value);
                err = DATASTORE_OK;
                break;
            }
            case DATASTORE_TYPE_INT8:
            {
                int8_t value = 0;
                datastore_get_int8(store, id, instance, &value);
                snprintf(buffer, len, "%d", value);
                err = DATASTORE_OK;
                break;
            }
            case DATASTORE_TYPE_INT32:
            {
                int32_t value = 0;
                datastore_get_int32(store, id, instance, &value);
                snprintf(buffer, len, "%d", value);
                err = DATASTORE_OK;
                break;
            }
            case DATASTORE_TYPE_FLOAT:
            {
                float value = 0.0f;
                datastore_get_float(store, id, instance, &value);
                snprintf(buffer, len, "%f", value);
                err = DATASTORE_OK;
                break;
            }
            case DATASTORE_TYPE_DOUBLE:
            {
                double value = 0.0;
                datastore_get_double(store, id, instance, &value);
                snprintf(buffer, len, "%lf", value);
                err = DATASTORE_OK;
                break;
            }
            case DATASTORE_TYPE_STRING:
            {
                // hope that the supplied buffer is big enough...
                //ESP_LOGI(TAG, "len %d, INDEX[id].size %d", len, INDEX[id].size);
                assert(len >= INDEX[id].size);
                datastore_get_string(store, id, instance, buffer);
                err = DATASTORE_OK;
                break;
            }
            default:
                ESP_LOGE(TAG, "unhandled type %d", INDEX[id].type);
                err = DATASTORE_ERROR_INVALID_TYPE;
                break;
        }
    }
    else
    {
        ESP_LOGE(TAG, "invalid datastore ID %d", id);
        err = DATASTORE_ERROR_INVALID_ID;
    }
    return err;
}

datastore_error_t datastore_dump(const datastore_t * store)
{
    datastore_error_t err = DATASTORE_ERROR_UNKNOWN;
    if ((err = _is_init(store)) == DATASTORE_OK)
    {
        for (datastore_id_t id = 0; id < DATASTORE_ID_LAST; ++id)
        {
            err = DATASTORE_OK;
            for (instance_id_t instance = 0; err == DATASTORE_OK && instance < INDEX[id].num_instances; ++instance)
            {
                char value[256] = "";
                err = _to_string(store, id, instance, value, 256);
                ESP_LOGI(TAG, LOG_COLOR(LOG_COLOR_PURPLE)"%2d %-40s %d %s", id, INDEX[id].name, instance, value);
            }
            if (err != DATASTORE_OK)
            {
                ESP_LOGE(TAG, "Error %d", err);
            }
        }
    }
    return err;
}


// NEW API

// String instances are handled differently, using a linked list of variable
// size strings. All other types are fixed size for fast lookup of instances.

typedef struct
{
    datastore2_resource_id_t id;   // not necessary? Keep as a check
    const char * name;
    datastore_type_t type;
    datastore2_instance_id_t num_instances;
    void * data;   // pointer to first byte of first instance
    size_t size;   // per instance size
    bool managed;  // data allocation is managed by API
} index_row_t;

typedef struct
{
    index_row_t * index_rows;
    size_t index_size;
} private2_t;

#define _error(f, ...) do { fprintf(stdout /*stderr*/, f"\n", ##__VA_ARGS__); } while (0)
#define _debug(f, ...)
//#define _debug(f, ...) do { fprintf(stdout, f"\n", ##__VA_ARGS__); } while (0)

datastore2_t * datastore2_create(void)
{
    datastore2_t * datastore = NULL;
    private2_t * private = malloc(sizeof(*private));
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

void datastore2_free(datastore2_t ** datastore)
{
    if (datastore != NULL && (*datastore != NULL))
    {
        private2_t * private = (private2_t *)(*datastore)->private_data;
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

static datastore_error_t _add_resource(datastore2_t * datastore, datastore2_resource_id_t resource_id, datastore_type_t type, uint32_t num_instances, void * data, size_t size, bool managed)
{
    datastore_error_t err = DATASTORE_ERROR_UNKNOWN;
    if (datastore != NULL)
    {
        private2_t * private = (private2_t *)datastore->private_data;
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
                                    err = DATASTORE_ERROR_OUT_OF_MEMORY;
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

                            err = DATASTORE_OK;
                        }
                        else
                        {
                            err = DATASTORE_ERROR_NULL_POINTER;
                            _error("data is NULL");
                        }
                    }
                    else
                    {
                        err = DATASTORE_ERROR_INVALID_INSTANCE;
                        _error("%d instances is invalid", num_instances);
                    }
                }
                else
                {
                    err = DATASTORE_ERROR_INVALID_TYPE;
                    _error("resource type %d is invalid", type);
                }
            }
            else
            {
                err = DATASTORE_ERROR_INVALID_ID;
                _error("resource ID %d is invalid", resource_id);
            }
        }
        else
        {
            _error("private is NULL");
            err = DATASTORE_ERROR_NULL_POINTER;
        }
    }
    else
    {
        err = DATASTORE_ERROR_NULL_POINTER;
        _error("datastore is NULL");
    }
out:
    return err;
}

datastore_resource_t datastore2_create_resource(datastore_type_t type, uint32_t num_instances)
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

datastore_resource_t datastore2_create_string_resource(size_t length, uint32_t num_instances)
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

datastore_error_t datastore2_add_resource(datastore2_t * datastore, datastore2_resource_id_t resource_id, const datastore_resource_t resource)
{
    return _add_resource(datastore, resource_id, resource.type, resource.num_instances, resource.data, resource.size, resource._managed);
}

datastore_error_t datastore2_add_fixed_length_resource(datastore2_t * datastore, datastore2_resource_id_t resource_id, datastore_type_t type, uint32_t num_instances)
{
    datastore_error_t err = DATASTORE_ERROR_UNKNOWN;
    size_t size = TYPE_SIZES[type];
    if (type != DATASTORE_TYPE_STRING)
    {
        void * data = malloc(size * num_instances);
        if (data != NULL)
        {
            memset(data, 0, size * num_instances);
            err = _add_resource(datastore, resource_id, type, num_instances, data, size, true);
            if (err != DATASTORE_OK)
            {
                free(data);
                data = NULL;
            }
        }
        else
        {
            err = DATASTORE_ERROR_NULL_POINTER;
            _error("malloc returned NULL");
        }
    }
    else
    {
        err = DATASTORE_ERROR_INVALID_TYPE;
        _error("resource type %d is invalid", type);
    }
    return err;
}

datastore_error_t datastore2_add_string_resource(datastore2_t * datastore, datastore2_resource_id_t resource_id, uint32_t num_instances, size_t length)
{
    datastore_error_t err = DATASTORE_ERROR_UNKNOWN;
    size_t size = length;
    void * data = malloc(size * num_instances);
    if (data != NULL)
    {
        memset(data, 0, size * num_instances);
        err = _add_resource(datastore, resource_id, DATASTORE_TYPE_STRING, num_instances, data, size, true);
        if (err != DATASTORE_OK)
        {
            free(data);
            data = NULL;
        }
    }
    else
    {
        err = DATASTORE_ERROR_NULL_POINTER;
        _error("malloc returned NULL");
    }
    return err;
}

static datastore_error_t _set_value2(const datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, const void * value, size_t size, datastore_type_t expected_type)
{
    _debug("_set_value2: id %d, instance %d, value %p, size %d, expected_type %d", id, instance, value, size, expected_type);
    datastore_error_t err = DATASTORE_ERROR_UNKNOWN;
    if (datastore != NULL)
    {
        private2_t * private = (private2_t *)datastore->private_data;
        if (private != NULL)
        {
            if (id >= 0 && id < private->index_size / sizeof(index_row_t))
            {
                // check type
                if (private->index_rows[id].type == expected_type)
                {
                    // check instance
                    if (/*instance >= 0 &&*/ instance < private->index_rows[id].num_instances)
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

                                // TODO: call any registered callbacks with new value
                                err = DATASTORE_OK;
                            }
                            else
                            {
                                _error("_set_value2: value is NULL");
                                err = DATASTORE_ERROR_NULL_POINTER;
                            }
                        }
                        else
                        {
                            _error("_set_value2: value size %lu exceeds allocated size %lu", size, private->index_rows[id].size);
                            err = DATASTORE_ERROR_TOO_LARGE;
                        }
                    }
                    else
                    {
                        _error("_set_value2: instance %d is invalid", instance);
                        err = DATASTORE_ERROR_INVALID_INSTANCE;
                    }
                }
                else
                {
                    _error("_set_value2: bad type %d (expected %d)", private->index_rows[id].type, expected_type);
                    err = DATASTORE_ERROR_INVALID_TYPE;
                }
            }
            else
            {
                _error("_set_value2: bad id %d", id);
                err = DATASTORE_ERROR_INVALID_ID;
            }
        }
        else
        {
            _error("_set_value2: private is NULL");
            err = DATASTORE_ERROR_NULL_POINTER;
        }
    }
    else
    {
        _error("_set_value2: datastore is NULL");
        err = DATASTORE_ERROR_NULL_POINTER;
    }
    return err;
}

datastore_error_t datastore2_set_bool(datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, bool value)
{
    return _set_value2(datastore, id, instance, &value, sizeof(bool), DATASTORE_TYPE_BOOL);
}

datastore_error_t datastore2_set_uint8(datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, uint8_t value)
{
    return _set_value2(datastore, id, instance, &value, sizeof(uint8_t), DATASTORE_TYPE_UINT8);
}

datastore_error_t datastore2_set_uint32(datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, uint32_t value)
{
    return _set_value2(datastore, id, instance, &value, sizeof(uint32_t), DATASTORE_TYPE_UINT32);
}

datastore_error_t datastore2_set_int8(datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, int8_t value)
{
    return _set_value2(datastore, id, instance, &value, sizeof(int8_t), DATASTORE_TYPE_INT8);
}

datastore_error_t datastore2_set_int32(datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, int32_t value)
{
    return _set_value2(datastore, id, instance, &value, sizeof(int32_t), DATASTORE_TYPE_INT32);
}

datastore_error_t datastore2_set_float(datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, float value)
{
    return _set_value2(datastore, id, instance, &value, sizeof(float), DATASTORE_TYPE_FLOAT);
}

datastore_error_t datastore2_set_double(datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, double value)
{
    return _set_value2(datastore, id, instance, &value, sizeof(double), DATASTORE_TYPE_DOUBLE);
}

datastore_error_t datastore2_set_string(datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, const char * value)
{
    return _set_value2(datastore, id, instance, value, strlen(value) + 1, DATASTORE_TYPE_STRING);
}

static datastore_error_t _get_value2(const datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, void * value, datastore_type_t expected_type)
{
    _debug("_get_value2: id %d, instance %d, value %p, expected_type %d", id, instance, value, expected_type);
    datastore_error_t err = DATASTORE_ERROR_UNKNOWN;
    if (datastore != NULL)
    {
        private2_t * private = (private2_t *)datastore->private_data;
        if (private != NULL)
        {
            if (id >= 0 && id < private->index_size / sizeof(index_row_t))
            {
                // check type
                if (private->index_rows[id].type == expected_type)
                {
                    // check instance
                    if (/*instance >= 0 &&*/ instance < private->index_rows[id].num_instances)
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
                            err = DATASTORE_OK;
                        }
                        else
                        {
                            _error("_get_value2: value is NULL");
                            err = DATASTORE_ERROR_NULL_POINTER;
                        }
                    }
                    else
                    {
                        _error("_get_value2: instance %d is invalid", instance);
                        err = DATASTORE_ERROR_INVALID_INSTANCE;
                    }
                }
                else
                {
                    _error("_get_value2: bad type %d (expected %d)", private->index_rows[id].type, expected_type);
                    err = DATASTORE_ERROR_INVALID_TYPE;
                }
            }
            else
            {
                _error("_get_value2: bad id %d", id);
                err = DATASTORE_ERROR_INVALID_ID;
            }
        }
        else
        {
            _error("_get_value2: private is NULL");
            err = DATASTORE_ERROR_NULL_POINTER;
        }
    }
    else
    {
        _error("_get_value2: datastore is NULL");
        err = DATASTORE_ERROR_NULL_POINTER;
    }
    return err;
}

datastore_error_t datastore2_get_bool(const datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, bool * value)
{
    return _get_value2(datastore, id, instance, value, DATASTORE_TYPE_BOOL);
}

datastore_error_t datastore2_get_uint8(const datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, uint8_t * value)
{
    return _get_value2(datastore, id, instance, value, DATASTORE_TYPE_UINT8);
}

datastore_error_t datastore2_get_uint32(const datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, uint32_t * value)
{
    return _get_value2(datastore, id, instance, value, DATASTORE_TYPE_UINT32);
}

datastore_error_t datastore2_get_int8(const datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, int8_t * value)
{
    return _get_value2(datastore, id, instance, value, DATASTORE_TYPE_INT8);
}

datastore_error_t datastore2_get_int32(const datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, int32_t * value)
{
    return _get_value2(datastore, id, instance, value, DATASTORE_TYPE_INT32);
}

datastore_error_t datastore2_get_float(const datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, float * value)
{
    return _get_value2(datastore, id, instance, value, DATASTORE_TYPE_FLOAT);
}

datastore_error_t datastore2_get_double(const datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, double * value)
{
    return _get_value2(datastore, id, instance, value, DATASTORE_TYPE_DOUBLE);
}

datastore_error_t datastore2_get_string(const datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, char * value)
{
    return _get_value2(datastore, id, instance, value, DATASTORE_TYPE_STRING);
}

