#include <gtest/gtest.h>
#include "datastore.h"

typedef enum {
    RESOURCE_INVALID = -1,
    RESOURCE0 = 0,
    RESOURCE1,
    RESOURCE2,
    RESOURCE3,
    RESOURCE4,
    RESOURCE5,
    RESOURCE6,
    RESOURCE7,
} resource_id;

TEST(DatastoreTest, create_and_free) {
    datastore_t * ds = datastore_create();
    ASSERT_TRUE(NULL != ds);
    datastore_free(&ds);
    EXPECT_EQ(NULL, ds);
}

TEST(DatastoreTest, free_invalid) {
    datastore_free(NULL);
    datastore_t * ds = datastore_create();
    datastore_free(&ds);
    datastore_free(&ds);
}

TEST(DatastoreTest, add_managed_scalar_uint32) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));
    datastore_free(&ds);
}

TEST(DatastoreTest, add_managed_scalar_string) {
    datastore_t * ds = datastore_create();
    const uint32_t STRING_LEN = 32;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_string_resource(ds, RESOURCE0, 1, STRING_LEN));
    datastore_free(&ds);
}

TEST(DatastoreTest, add_managed_tabular_uint32) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1000));
    datastore_free(&ds);
}

TEST(DatastoreTest, add_managed_tabular_string) {
    datastore_t * ds = datastore_create();
    const uint32_t STRING_LEN = 32;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_string_resource(ds, RESOURCE0, 100, STRING_LEN));
    datastore_free(&ds);
}

TEST(DatastoreTest, add_managed_scalar_uint32_to_null_datastore) {
    EXPECT_EQ(DATASTORE_STATUS_ERROR_NULL_POINTER, datastore_add_fixed_length_resource(NULL, RESOURCE0, DATASTORE_TYPE_UINT32, 1));
}

TEST(DatastoreTest, add_managed_with_invalid_id) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_ID, datastore_add_fixed_length_resource(ds, RESOURCE_INVALID, DATASTORE_TYPE_UINT32, 1));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_ID, datastore_add_fixed_length_resource(ds, -1000, DATASTORE_TYPE_UINT32, 1));
    datastore_free(&ds);
}

TEST(DatastoreTest, add_managed_with_invalid_type) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_TYPE, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_LAST, 1));
    datastore_free(&ds);
}

TEST(DatastoreTest, add_managed_with_invalid_number_of_instances) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 0));
    datastore_free(&ds);
}

TEST(DatastoreTest, add_managed_string_with_wrong_function) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_TYPE, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_STRING, 1));
    datastore_free(&ds);
}

TEST(DatastoreTest, add_duplicate_resource0) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_ID, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_INT32, 1));
    datastore_free(&ds);
}

TEST(DatastoreTest, add_duplicate_resource1) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE1, DATASTORE_TYPE_UINT32, 1));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_ID, datastore_add_fixed_length_resource(ds, RESOURCE1, DATASTORE_TYPE_INT32, 1));
    datastore_free(&ds);
}

TEST(DatastoreTest, managed_scalar_uint32_defaults_to_zero) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));

    uint32_t value = 42;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, RESOURCE0, 0, &value));
    EXPECT_EQ(0, value);
    datastore_free(&ds);
}

TEST(DatastoreTest, get_instance_of_scalar_out_of_range) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));

    uint32_t value = 42;
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_get_uint32(ds, RESOURCE0, 1, &value));
    EXPECT_EQ(42, value);
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_get_uint32(ds, RESOURCE0, -1, &value));
    EXPECT_EQ(42, value);
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_get_uint32(ds, RESOURCE0, 100, &value));
    EXPECT_EQ(42, value);
    datastore_free(&ds);
}

TEST(DatastoreTest, set_instance_of_scalar_out_of_range) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));

    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_set_uint32(ds, RESOURCE0, 1, 42));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_set_uint32(ds, RESOURCE0, -1, 42));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_set_uint32(ds, RESOURCE0, 100, 42));
    datastore_free(&ds);
}

TEST(DatastoreTest, managed_scalar_uint32_set_and_get) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));

    uint32_t value = 0;

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE0, 0, 17));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, RESOURCE0, 0, &value));
    EXPECT_EQ(17, value);
    datastore_free(&ds);
}

TEST(DatastoreTest, managed_scalar_uint32_set_and_get_multiple) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));

    uint32_t value = 0;

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE0, 0, UINT32_MAX));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, RESOURCE0, 0, &value));
    EXPECT_EQ(UINT32_MAX, value);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE0, 0, 0));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, RESOURCE0, 0, &value));
    EXPECT_EQ(0, value);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE0, 0, 1234567890));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, RESOURCE0, 0, &value));
    EXPECT_EQ(1234567890, value);
    datastore_free(&ds);
}

TEST(DatastoreTest, managed_tabular_uint32_defaults_to_zero) {
    datastore_t * ds = datastore_create();
    const int NUM_INSTANCES = 1000;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, NUM_INSTANCES));

    for (int i = 0; i < NUM_INSTANCES; ++i) {
        uint32_t value = 42;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, RESOURCE0, i, &value));
        EXPECT_EQ(0, value);
    }
    datastore_free(&ds);
}

TEST(DatastoreTest, get_instance_of_tabular_out_of_range) {
    datastore_t * ds = datastore_create();
    const int NUM_INSTANCES = 1000;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, NUM_INSTANCES));

    uint32_t value = 42;
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_get_uint32(ds, RESOURCE0, NUM_INSTANCES, &value));
    EXPECT_EQ(42, value);
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_get_uint32(ds, RESOURCE0, NUM_INSTANCES + 1, &value));
    EXPECT_EQ(42, value);
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_get_uint32(ds, RESOURCE0, NUM_INSTANCES + 1000, &value));
    EXPECT_EQ(42, value);
    datastore_free(&ds);
}

TEST(DatastoreTest, set_instance_of_tabular_out_of_range) {
    datastore_t * ds = datastore_create();
    const int NUM_INSTANCES = 1000;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, NUM_INSTANCES));

    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_set_uint32(ds, RESOURCE0, NUM_INSTANCES, 42));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_set_uint32(ds, RESOURCE0, NUM_INSTANCES + 1, 42));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_set_uint32(ds, RESOURCE0, NUM_INSTANCES + 1000, 42));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_set_uint32(ds, RESOURCE0, -1, 42));
    datastore_free(&ds);
}

TEST(DatastoreTest, get_unknown_resources) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_ID, datastore_get_bool(ds, RESOURCE0, 0, NULL));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_ID, datastore_get_uint32(ds, RESOURCE1, 4, NULL));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_ID, datastore_get_string(ds, RESOURCE2, 0, NULL, 4));
    datastore_free(&ds);
}

TEST(DatastoreTest, get_resource_with_wrong_function) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE1, DATASTORE_TYPE_BOOL, 1));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, RESOURCE2, DATASTORE_TYPE_FLOAT, 1));

    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_TYPE, datastore_get_bool(ds, RESOURCE0, 0, NULL));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_TYPE, datastore_get_int32(ds, RESOURCE0, 0, NULL));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_TYPE, datastore_get_uint32(ds, RESOURCE1, 4, NULL));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_TYPE, datastore_get_uint8(ds, RESOURCE1, 4, NULL));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_TYPE, datastore_get_string(ds, RESOURCE2, 0, NULL, 4));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_TYPE, datastore_get_double(ds, RESOURCE2, 0, NULL));
    datastore_free(&ds);
}

TEST(DatastoreTest, test_set_with_null_datastore) {
    EXPECT_EQ(DATASTORE_STATUS_ERROR_NULL_POINTER, datastore_set_bool(NULL, 0, 0, true));
}

TEST(DatastoreTest, test_set_string_with_null_value) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_ERROR_NULL_POINTER, datastore_set_string(ds, 0, 0, NULL));
    datastore_free(&ds);
}

TEST(DatastoreTest, test_scalar_bool) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = RESOURCE3;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_BOOL, 1));
    bool value = false;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_bool(ds, ID, 0, &value));
    EXPECT_FALSE(value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_bool(ds, ID, 0, true));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_bool(ds, ID, 0, &value));
    EXPECT_TRUE(value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_bool(ds, ID, 0, false));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_bool(ds, ID, 0, &value));
    EXPECT_FALSE(value);
    datastore_free(&ds);
}

TEST(DatastoreTest, test_scalar_uint8) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 99;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_UINT8, 1));
    uint8_t value = 42;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint8(ds, ID, 0, &value));
    EXPECT_EQ(0, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint8(ds, ID, 0, UINT8_MAX));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint8(ds, ID, 0, &value));
    EXPECT_EQ(UINT8_MAX, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint8(ds, ID, 0, 0));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint8(ds, ID, 0, &value));
    EXPECT_EQ(0, value);
    datastore_free(&ds);
}

TEST(DatastoreTest, test_scalar_uint32) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 1234;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_UINT32, 1));
    uint32_t value = 42;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, ID, 0, &value));
    EXPECT_EQ(0, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, ID, 0, UINT32_MAX));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, ID, 0, &value));
    EXPECT_EQ(UINT32_MAX, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, ID, 0, 0));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, ID, 0, &value));
    EXPECT_EQ(0, value);
    datastore_free(&ds);
}

TEST(DatastoreTest, test_scalar_int8) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 99;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_INT8, 1));
    int8_t value = 42;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int8(ds, ID, 0, &value));
    EXPECT_EQ(0, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int8(ds, ID, 0, INT8_MAX));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int8(ds, ID, 0, &value));
    EXPECT_EQ(INT8_MAX, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int8(ds, ID, 0, 0));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int8(ds, ID, 0, &value));
    EXPECT_EQ(0, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int8(ds, ID, 0, INT8_MIN));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int8(ds, ID, 0, &value));
    EXPECT_EQ(INT8_MIN, value);
    datastore_free(&ds);
}

TEST(DatastoreTest, test_scalar_int32) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 1234;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_INT32, 1));
    int32_t value = 42;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int32(ds, ID, 0, &value));
    EXPECT_EQ(0, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int32(ds, ID, 0, INT32_MAX));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int32(ds, ID, 0, &value));
    EXPECT_EQ(INT32_MAX, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int32(ds, ID, 0, 0));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int32(ds, ID, 0, &value));
    EXPECT_EQ(0, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int32(ds, ID, 0, INT32_MIN));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int32(ds, ID, 0, &value));
    EXPECT_EQ(INT32_MIN, value);
    datastore_free(&ds);
}

TEST(DatastoreTest, test_scalar_float) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 1;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_FLOAT, 1));
    float value = 42.7f;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_float(ds, ID, 0, &value));
    EXPECT_EQ(0.0f, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_float(ds, ID, 0, FLT_MAX));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_float(ds, ID, 0, &value));
    EXPECT_EQ(FLT_MAX, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_float(ds, ID, 0, 0.0f));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_float(ds, ID, 0, &value));
    EXPECT_EQ(0.0f, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_float(ds, ID, 0, FLT_MIN));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_float(ds, ID, 0, &value));
    EXPECT_EQ(FLT_MIN, value);
    datastore_free(&ds);
}

TEST(DatastoreTest, test_scalar_double) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 42;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_DOUBLE, 1));
    double value = 42.7;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_double(ds, ID, 0, &value));
    EXPECT_EQ(0.0f, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_double(ds, ID, 0, DBL_MAX));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_double(ds, ID, 0, &value));
    EXPECT_EQ(DBL_MAX, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_double(ds, ID, 0, 0.0));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_double(ds, ID, 0, &value));
    EXPECT_EQ(0.0, value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_double(ds, ID, 0, DBL_MIN));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_double(ds, ID, 0, &value));
    EXPECT_EQ(DBL_MIN, value);
    datastore_free(&ds);
}

TEST(DatastoreTest, test_scalar_string) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 42;
    const uint32_t STRING_LEN = 8;  // inc. 1 for null terminator
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_string_resource(ds, ID, 1, STRING_LEN));
    char value[STRING_LEN] = "";
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_string(ds, ID, 0, value, sizeof(value)));
    EXPECT_STREQ("", value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_string(ds, ID, 0, "abcdefg"));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_string(ds, ID, 0, value, sizeof(value)));
    EXPECT_STREQ("abcdefg", value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_string(ds, ID, 0, ""));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_string(ds, ID, 0, value, sizeof(value)));
    EXPECT_STREQ("", value);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_string(ds, ID, 0, "1234567"));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_string(ds, ID, 0, value, sizeof(value)));
    EXPECT_STREQ("1234567", value);
    datastore_free(&ds);
}

TEST(DatastoreTest, test_scalar_string_exceeds_allocated_length) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 4;
    const uint32_t STRING_LEN = 8;  // inc. 1 for null terminator
    char value[STRING_LEN] = "";
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_string_resource(ds, ID, 1, STRING_LEN));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_TOO_LARGE, datastore_set_string(ds, ID, 0, "abcdefghijkl"));
    EXPECT_STREQ("", value);
    datastore_free(&ds);
}

TEST(DatastoreTest, test_tabular_bool) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 199;
    const uint32_t NUM_INSTANCES = 200;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_BOOL, NUM_INSTANCES));

    // check defaults
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        bool value = false;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_bool(ds, ID, i, &value));
        EXPECT_FALSE(value);
    }

    // set all to predictable values
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_bool(ds, ID, i, (i / 3) % 2 ? true : false));
    }

    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        bool value = false;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_bool(ds, ID, i, &value));
        EXPECT_EQ((i / 3) % 2 ? true : false, value);
    }
    datastore_free(&ds);
}

TEST(DatastoreTest, test_tabular_uint8) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 55;
    const uint32_t NUM_INSTANCES = 1000;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_UINT8, NUM_INSTANCES));

    // check defaults
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        uint8_t value = 0;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint8(ds, ID, i, &value));
        EXPECT_FALSE(value);
    }

    // set all to predictable values
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint8(ds, ID, i, i * i + 3 * i - 7));
    }

    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        uint8_t value = 0;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint8(ds, ID, i, &value));
        EXPECT_EQ((uint8_t)(i * i + 3 * i - 7), value);
    }
    datastore_free(&ds);
}

TEST(DatastoreTest, test_tabular_uint32) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 55;
    const uint32_t NUM_INSTANCES = 1000;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_UINT32, NUM_INSTANCES));

    // check defaults
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        uint32_t value = 0;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, ID, i, &value));
        EXPECT_FALSE(value);
    }

    // set all to predictable values
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, ID, i, 3 * i * i * i + 2 * i * i + 5 * i - 16));
    }

    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        uint32_t value = 0;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, ID, i, &value));
        EXPECT_EQ((uint32_t)(3 * i * i * i + 2 * i * i + 5 * i - 16), value);
    }
    datastore_free(&ds);
}

TEST(DatastoreTest, test_tabular_int8) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 55;
    const int32_t NUM_INSTANCES = 1000;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_INT8, NUM_INSTANCES));

    // check defaults
    for (int32_t i = 0; i < NUM_INSTANCES; ++i) {
        int8_t value = 0;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int8(ds, ID, i, &value));
        EXPECT_FALSE(value);
    }

    // set all to predictable values
    for (int32_t i = 0; i < NUM_INSTANCES; ++i) {
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int8(ds, ID, i, i * i + 3 * i - 7));
    }

    for (int32_t i = 0; i < NUM_INSTANCES; ++i) {
        int8_t value = 0;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int8(ds, ID, i, &value));
        EXPECT_EQ((int8_t)(i * i + 3 * i - 7), value);
    }
    datastore_free(&ds);
}

TEST(DatastoreTest, test_tabular_int32) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 55;
    const int32_t NUM_INSTANCES = 1000;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_INT32, NUM_INSTANCES));

    // check defaults
    for (int32_t i = 0; i < NUM_INSTANCES; ++i) {
        int32_t value = 0;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int32(ds, ID, i, &value));
        EXPECT_FALSE(value);
    }

    // set all to predictable values
    for (int32_t i = 0; i < NUM_INSTANCES; ++i) {
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int32(ds, ID, i, 3 * i * i * i + 2 * i * i + 5 * i - 16));
    }

    for (int32_t i = 0; i < NUM_INSTANCES; ++i) {
        int32_t value = 0;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int32(ds, ID, i, &value));
        EXPECT_EQ((int32_t)(3 * i * i * i + 2 * i * i + 5 * i - 16), value);
    }
    datastore_free(&ds);
}

TEST(DatastoreTest, test_tabular_float) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 199;
    const uint32_t NUM_INSTANCES = 19;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_FLOAT, NUM_INSTANCES));

    // check defaults
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        float value = 0.0f;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_float(ds, ID, i, &value));
        EXPECT_EQ(0.0f, value);
    }

    // set all to predictable values
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_float(ds, ID, i, 0.91f * i * i + 145.67f * i - 953.213f));
    }

    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        float value = 0.0f;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_float(ds, ID, i, &value));
        EXPECT_EQ(0.91f * i * i + 145.67f * i - 953.213f, value);
    }
    datastore_free(&ds);
}

TEST(DatastoreTest, test_tabular_double) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 199;
    const uint32_t NUM_INSTANCES = 521;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_DOUBLE, NUM_INSTANCES));

    // check defaults
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        double value = 0.0;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_double(ds, ID, i, &value));
        EXPECT_EQ(0.0, value);
    }

    // set all to predictable values
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_double(ds, ID, i, 67.91f * i * i + 135.67f * i - 953.213f));
    }

    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        double value = 0.0;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_double(ds, ID, i, &value));
        EXPECT_EQ(67.91f * i * i + 135.67f * i - 953.213f, value);
    }
    datastore_free(&ds);
}

TEST(DatastoreTest, test_tabular_string) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 5432;
    const uint32_t NUM_INSTANCES = 21;
    const uint32_t STRING_LEN = 8;  // inc. 1 for null terminator
    char value[STRING_LEN] = "";
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_string_resource(ds, ID, NUM_INSTANCES, STRING_LEN));

    // check defaults
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        memset(value, 0, STRING_LEN);
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_string(ds, ID, i, value, sizeof(value)));
        EXPECT_STREQ("", value);
    }

    // set all to predictable values
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        char s[STRING_LEN] = "";
        snprintf(s, STRING_LEN, "abc-%03d", i);
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_string(ds, ID, i, s));
    }

    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        memset(value, 0, STRING_LEN);
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_string(ds, ID, i, value, sizeof(value)));
        char s[STRING_LEN] = "";
        snprintf(s, STRING_LEN, "abc-%03d", i);
        EXPECT_STREQ(s, value);
    }
    datastore_free(&ds);
}

TEST(DatastoreTest, test_tabular_string_exceeds_allocated_length) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 43;
    const uint32_t NUM_INSTANCES = 54;
    const uint32_t STRING_LEN = 64;  // inc. 1 for null terminator
    char value[STRING_LEN] = "";
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_string_resource(ds, ID, NUM_INSTANCES, STRING_LEN));

    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        char * s = (char *)malloc(STRING_LEN + 1 + i);
        memset(s, 'A', STRING_LEN + i);
        s[STRING_LEN + i] = '\0';
        EXPECT_EQ(DATASTORE_STATUS_ERROR_TOO_LARGE, datastore_set_string(ds, ID, i, s));
        free(s);
    }
    datastore_free(&ds);
}

TEST(DatastoreTest, test_create_and_add_resources) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE0, datastore_create_resource(DATASTORE_TYPE_UINT8, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE1, datastore_create_resource(DATASTORE_TYPE_INT32, 7)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE2, datastore_create_string_resource(32, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE3, datastore_create_string_resource(8, 8)));
    datastore_free(&ds);
}

TEST(DatastoreTest, test_add_resource_with_null_data) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_ERROR_NULL_POINTER, datastore_add_resource(ds, RESOURCE0, (datastore_resource_t){.data=NULL, .size=0, .type=DATASTORE_TYPE_UINT32, .num_instances=1 }));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_NULL_POINTER, datastore_add_resource(ds, RESOURCE0, (datastore_resource_t){.data=NULL, .size=4, .type=DATASTORE_TYPE_UINT32, .num_instances=1 }));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_NULL_POINTER, datastore_add_resource(ds, RESOURCE0, (datastore_resource_t){.data=NULL, .size=4, .type=DATASTORE_TYPE_UINT32, .num_instances=10 }));
    datastore_free(&ds);
}

TEST(DatastoreTest, test_create_resource_with_invalid_type) {
    datastore_t * ds = datastore_create();
    datastore_resource_t resource = datastore_create_resource(DATASTORE_TYPE_INVALID, 1);
    EXPECT_EQ(NULL, resource.data);
    EXPECT_EQ(0, resource.size);
    datastore_free(&ds);
}

TEST(DatastoreTest, test_add_static_resources) {
    datastore_t * ds = datastore_create();
    struct st {
        uint32_t d0;
        int8_t d1[3];
        char s0[32];
        char s1[3][4];
    } st = { 42, {-7, 0, 5}, "hello", { "abc", "def", "ghi" } };
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE0, (datastore_resource_t){.data=&st.d0, .size=sizeof(st.d0), .type=DATASTORE_TYPE_UINT32, .num_instances=1 }));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE1, (datastore_resource_t){.data=&st.d1, .size=sizeof(st.d1[0]), .type=DATASTORE_TYPE_INT8, .num_instances=3 }));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE2, (datastore_resource_t){.data=st.s0, .size=sizeof(st.s0), .type=DATASTORE_TYPE_STRING, .num_instances=1 }));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE3, (datastore_resource_t){.data=st.s1, .size=sizeof(st.s1[0]), .type=DATASTORE_TYPE_STRING, .num_instances=3 }));

    uint32_t d0 = 0;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, RESOURCE0, 0, &d0));
    EXPECT_EQ(st.d0, d0);

    int8_t d1 = 0;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int8(ds, RESOURCE1, 0, &d1));
    EXPECT_EQ(st.d1[0], d1);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int8(ds, RESOURCE1, 1, &d1));
    EXPECT_EQ(st.d1[1], d1);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int8(ds, RESOURCE1, 2, &d1));
    EXPECT_EQ(st.d1[2], d1);

    datastore_free(&ds);
}

namespace detail {
    struct CallbackParameterRecord {
        CallbackParameterRecord() : datastore(nullptr), resource_id(-1), instance_id(-1), context(nullptr) {}
        const datastore_t * datastore;
        datastore_resource_id_t resource_id;
        datastore_instance_id_t instance_id;
        void * context;
    };

    struct CallbackRecord {
        CallbackRecord() : counter(0), last(), value1(0), value2(0) {}
        int counter;
        CallbackParameterRecord last;
        uint32_t value1;
        uint32_t value2;
    };

    static void callback(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, void * context) {
        std::cout << "callback: " << "datastore " << datastore << ", id " << id << ", instance " << instance << ", context " << context << std::endl;
        CallbackRecord * record = static_cast<CallbackRecord *>(context);
        ++record->counter;
        record->last.datastore = datastore;
        record->last.resource_id = id;
        record->last.instance_id = instance;
        record->last.context = context;
    }
}

TEST(DatastoreTest, test_set_callback) {
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 422;

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_UINT32, 10));

    detail::CallbackRecord record;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_set_callback(ds, ID, detail::callback, &record));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, ID, 0, 42));
    uint32_t value = 0;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, ID, 0, &value));

    EXPECT_EQ(1, record.counter);
    EXPECT_EQ(ds, record.last.datastore);
    EXPECT_EQ(ID, record.last.resource_id);
    EXPECT_EQ(0, record.last.instance_id);
    EXPECT_EQ(&record, record.last.context);
    EXPECT_EQ(42, value);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, ID, 5, 17));
    EXPECT_EQ(2, record.counter);
    EXPECT_EQ(ds, record.last.datastore);
    EXPECT_EQ(ID, record.last.resource_id);
    EXPECT_EQ(5, record.last.instance_id);
    EXPECT_EQ(&record, record.last.context);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, ID, 5, &value));
    EXPECT_EQ(17, value);

    datastore_free(&ds);
}

TEST(DatastoreTest, test_set_callback_with_null_datastore) {
    EXPECT_EQ(DATASTORE_STATUS_ERROR_NULL_POINTER, datastore_add_set_callback(NULL, 0, detail::callback, NULL));
}

TEST(DatastoreTest, test_set_callback_with_null_private) {
    datastore_t * ds = datastore_create();
    auto tmp = ds->private_data;
    ds->private_data = NULL;
    EXPECT_EQ(DATASTORE_STATUS_ERROR_NULL_POINTER, datastore_add_set_callback(ds, 0, detail::callback, NULL));
    ds->private_data = tmp;
    datastore_free(&ds);
}

TEST(DatastoreTest, test_set_callback_with_invalid_id) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_ID, datastore_add_set_callback(ds, 17, detail::callback, NULL));
    datastore_free(&ds);
}

namespace detail {
    struct SharedCallbackRecord {
        SharedCallbackRecord(int * shared) : shared(shared) {}
        CallbackRecord record;
        int * shared;
    };

    static void chained_callback1(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, void * context) {
        std::cout << "chained_callback1: " << "datastore " << datastore << ", id " << id << ", instance " << instance << ", context " << context << std::endl;
        SharedCallbackRecord * record = static_cast<SharedCallbackRecord *>(context);
        callback(datastore, id, instance, &record->record);
        *record->shared += 7;  // add 7
    }

    static void chained_callback2(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, void * context) {
        std::cout << "chained_callback2: " << "datastore " << datastore << ", id " << id << ", instance " << instance << ", context " << context << std::endl;
        SharedCallbackRecord * record = static_cast<SharedCallbackRecord *>(context);
        callback(datastore, id, instance, &record->record);
        *record->shared *= 2;  // double
    }

    static void chained_callback3(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, void * context) {
        std::cout << "chained_callback3: " << "datastore " << datastore << ", id " << id << ", instance " << instance << ", context " << context << std::endl;
        SharedCallbackRecord * record = static_cast<SharedCallbackRecord *>(context);
        callback(datastore, id, instance, &record->record);
        *record->shared -= 1;  // subtract 1
    }
}

TEST(DatastoreTest, test_set_callback_chain) {
    // add multiple callbacks, check they are all invoked in the correct order
    datastore_t * ds = datastore_create();
    const datastore_resource_id_t ID = 19;

    int shared = 19;
    detail::SharedCallbackRecord record1(&shared);
    detail::SharedCallbackRecord record2(&shared);
    detail::SharedCallbackRecord record3(&shared);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_UINT32, 10));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_set_callback(ds, ID, detail::chained_callback1, &record1));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_set_callback(ds, ID, detail::chained_callback2, &record2));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_set_callback(ds, ID, detail::chained_callback3, &record3));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, ID, 0, 42));

    EXPECT_EQ(1, record1.record.counter);
    EXPECT_EQ(ds, record1.record.last.datastore);
    EXPECT_EQ(ID, record1.record.last.resource_id);
    EXPECT_EQ(0, record1.record.last.instance_id);
    EXPECT_EQ(&record1, record1.record.last.context);
    EXPECT_EQ(1, record2.record.counter);
    EXPECT_EQ(ds, record2.record.last.datastore);
    EXPECT_EQ(ID, record2.record.last.resource_id);
    EXPECT_EQ(0, record2.record.last.instance_id);
    EXPECT_EQ(&record2, record2.record.last.context);
    EXPECT_EQ(1, record3.record.counter);
    EXPECT_EQ(ds, record3.record.last.datastore);
    EXPECT_EQ(ID, record3.record.last.resource_id);
    EXPECT_EQ(0, record3.record.last.instance_id);
    EXPECT_EQ(&record3, record3.record.last.context);

    uint32_t value = 0;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, ID, 0, &value));
    EXPECT_EQ(42, value);

    // shared_callback1 then shared_callback2 => (((x + 7) * 2) - 1)
    EXPECT_EQ(((19 + 7) * 2) - 1, shared);

    datastore_free(&ds);
}

namespace detail {
    static void callback_with_get(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, void * context) {
        std::cout << "callback: " << "datastore " << datastore << ", id " << id << ", instance " << instance << ", context " << context << std::endl;
        CallbackRecord * record = static_cast<CallbackRecord *>(context);
        ++record->counter;
        record->last.datastore = datastore;
        record->last.resource_id = id;
        record->last.instance_id = instance;
        record->last.context = context;
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(datastore, id, instance, &record->value1));
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(datastore, RESOURCE7, 0, &record->value2));
    }
}

TEST(DatastoreTest, test_get_from_set_callback) {
    // test that other value can be read from datastore by the callback
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE0, datastore_create_resource(DATASTORE_TYPE_UINT32, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE7, datastore_create_resource(DATASTORE_TYPE_UINT32, 1)));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE0, 0, 100));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE7, 0, 200));

    detail::CallbackRecord record;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_set_callback(ds, RESOURCE0, detail::callback_with_get, &record));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE0, 0, 42));

    EXPECT_EQ(1, record.counter);
    EXPECT_EQ(ds, record.last.datastore);
    EXPECT_EQ(RESOURCE0, record.last.resource_id);
    EXPECT_EQ(0, record.last.instance_id);
    EXPECT_EQ(&record, record.last.context);

    uint32_t value1 = 0;
    uint32_t value2 = 0;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, RESOURCE0, 0, &value1));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, RESOURCE7, 0, &value2));

    EXPECT_EQ(42, value1);
    EXPECT_EQ(record.value1, value1);
    EXPECT_EQ(record.value2, value2);

    datastore_free(&ds);
}

namespace detail {
    static void callback_with_set(const datastore_t * datastore, datastore_resource_id_t id, datastore_instance_id_t instance, void * context) {
        std::cout << "callback: " << "datastore " << datastore << ", id " << id << ", instance " << instance << ", context " << context << std::endl;
        CallbackRecord * record = static_cast<CallbackRecord *>(context);
        ++record->counter;
        record->last.datastore = datastore;
        record->last.resource_id = id;
        record->last.instance_id = instance;
        record->last.context = context;
        // to avoid an infinite loop, do not set the same resource!
        //EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(datastore, id, instance, record->value1));
        EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(datastore, RESOURCE7, 0, record->value2));
    }
}

TEST(DatastoreTest, test_set_from_set_callback) {
    // test that another value can be written to the datastore by the callback
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE0, datastore_create_resource(DATASTORE_TYPE_UINT32, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE7, datastore_create_resource(DATASTORE_TYPE_UINT32, 1)));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE0, 0, 100));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE7, 0, 200));

    detail::CallbackRecord record;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_set_callback(ds, RESOURCE0, detail::callback_with_set, &record));
    record.value1 = 0;
    record.value2 = 1023;  // resource2 should be set to this

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE0, 0, 42));

    EXPECT_EQ(1, record.counter);
    EXPECT_EQ(ds, record.last.datastore);
    EXPECT_EQ(RESOURCE0, record.last.resource_id);
    EXPECT_EQ(0, record.last.instance_id);
    EXPECT_EQ(&record, record.last.context);

    uint32_t value1 = 0;
    uint32_t value2 = 0;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, RESOURCE0, 0, &value1));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, RESOURCE7, 0, &value2));

    EXPECT_EQ(42, value1);
    EXPECT_EQ(1023, value2);
    EXPECT_EQ(record.value2, value2);

    datastore_free(&ds);
}

TEST(DatastoreTest, test_get_number_of_instances) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE0, datastore_create_resource(DATASTORE_TYPE_UINT32, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE1, datastore_create_resource(DATASTORE_TYPE_INT8, 11)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE2, datastore_create_resource(DATASTORE_TYPE_BOOL, 99)));

    EXPECT_EQ(1, datastore_num_instances(ds, RESOURCE0));
    EXPECT_EQ(11, datastore_num_instances(ds, RESOURCE1));
    EXPECT_EQ(99, datastore_num_instances(ds, RESOURCE2));

    EXPECT_EQ(0, datastore_num_instances(ds, RESOURCE3));
    EXPECT_EQ(0, datastore_num_instances(NULL, RESOURCE0));
    EXPECT_EQ(0, datastore_num_instances(NULL, RESOURCE3));

    datastore_free(&ds);
}

TEST(DatastoreTest, test_dump) {
    EXPECT_EQ(DATASTORE_STATUS_ERROR_NULL_POINTER, datastore_dump(NULL));

    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_dump(ds));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE0, datastore_create_resource(DATASTORE_TYPE_UINT32, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE1, datastore_create_resource(DATASTORE_TYPE_INT8, 2)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE2, datastore_create_resource(DATASTORE_TYPE_BOOL, 3)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE3, datastore_create_string_resource(32, 4)));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE0, 0, 42));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int8(ds, RESOURCE1, 0, 10));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int8(ds, RESOURCE1, 1, 20));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_bool(ds, RESOURCE2, 0, true));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_bool(ds, RESOURCE2, 1, true));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_bool(ds, RESOURCE2, 2, false));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_string(ds, RESOURCE3, 0, "first"));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_string(ds, RESOURCE3, 1, "second"));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_string(ds, RESOURCE3, 2, "third"));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_string(ds, RESOURCE3, 3, "fourth"));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_name(ds, RESOURCE0, "resource 0"));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_name(ds, RESOURCE1, "resource 1"));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_name(ds, RESOURCE2, "resource 2"));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_name(ds, RESOURCE3, "resource 3"));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_dump(ds));
    datastore_free(&ds);
}

TEST(DatastoreTest, test_increment_uint32) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE0, datastore_create_resource(DATASTORE_TYPE_UINT32, 10)));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE0, 0, 42));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_increment(ds, RESOURCE0, 0));
    uint32_t value = 0;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, RESOURCE0, 0, &value));
    EXPECT_EQ(43, value);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE0, 5, 17));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_increment(ds, RESOURCE0, 5));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_increment(ds, RESOURCE0, 5));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_uint32(ds, RESOURCE0, 5, &value));
    EXPECT_EQ(19, value);

    datastore_free(&ds);
}

// TODO: check on increment callback

TEST(DatastoreTest, test_increment_int8) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE0, datastore_create_resource(DATASTORE_TYPE_INT8, 10)));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int8(ds, RESOURCE0, 0, -42));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_increment(ds, RESOURCE0, 0));
    int8_t value = 0;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int8(ds, RESOURCE0, 0, &value));
    EXPECT_EQ(-41, value);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int8(ds, RESOURCE0, 5, 17));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_increment(ds, RESOURCE0, 5));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_increment(ds, RESOURCE0, 5));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_int8(ds, RESOURCE0, 5, &value));
    EXPECT_EQ(19, value);

    datastore_free(&ds);
}

TEST(DatastoreTest, test_increment_bool) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE0, datastore_create_resource(DATASTORE_TYPE_BOOL, 10)));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_bool(ds, RESOURCE0, 0, true));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_increment(ds, RESOURCE0, 0));
    bool value = true;
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_bool(ds, RESOURCE0, 0, &value));
    EXPECT_EQ(false, value);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_bool(ds, RESOURCE0, 5, false));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_increment(ds, RESOURCE0, 5));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_bool(ds, RESOURCE0, 5, &value));
    EXPECT_EQ(true, value);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_increment(ds, RESOURCE0, 5));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_bool(ds, RESOURCE0, 5, &value));
    EXPECT_EQ(false, value);

    datastore_free(&ds);
}

TEST(DatastoreTest, test_increment_invalid) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE0, datastore_create_resource(DATASTORE_TYPE_UINT32, 10)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE1, datastore_create_resource(DATASTORE_TYPE_FLOAT, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE2, datastore_create_resource(DATASTORE_TYPE_DOUBLE, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE3, datastore_create_string_resource(10, 1)));

    // invalid id
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_ID, datastore_increment(ds, RESOURCE4, 0));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_ID, datastore_increment(ds, -1, 0));

    // invalid instances
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_increment(ds, RESOURCE0, -1));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_increment(ds, RESOURCE0, 11));

    // invalid types
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_TYPE, datastore_increment(ds, RESOURCE1, 0));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_TYPE, datastore_increment(ds, RESOURCE2, 0));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_TYPE, datastore_increment(ds, RESOURCE3, 0));

    datastore_free(&ds);
}

TEST(DatastoreTest, test_set_and_get_name) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE0, datastore_create_resource(DATASTORE_TYPE_UINT32, 10)));

    EXPECT_EQ(DATASTORE_STATUS_ERROR_NULL_POINTER, datastore_set_name(NULL, RESOURCE0, ""));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_ID, datastore_set_name(ds, RESOURCE1, "name"));

    EXPECT_EQ(NULL, datastore_get_name(ds, RESOURCE0)); // default

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_name(ds, RESOURCE0, ""));
    EXPECT_STREQ("", datastore_get_name(ds, RESOURCE0));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_name(ds, RESOURCE0, "name1"));
    EXPECT_STREQ("name1", datastore_get_name(ds, RESOURCE0));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_name(ds, RESOURCE0, "abcdefghijklmnopqrstuvwxyz1234567890-_=+[{]}\\|;:'\",<.>/?!@#$%^&*()"));
    EXPECT_STREQ("abcdefghijklmnopqrstuvwxyz1234567890-_=+[{]}\\|;:'\",<.>/?!@#$%^&*()", datastore_get_name(ds, RESOURCE0));

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_name(ds, RESOURCE0, NULL));  // setting to NULL is OK
    EXPECT_EQ(NULL, datastore_get_name(ds, RESOURCE0));

    datastore_free(&ds);
}

TEST(DatastoreTest, test_get_as_string) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE0, datastore_create_resource(DATASTORE_TYPE_BOOL, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE1, datastore_create_resource(DATASTORE_TYPE_UINT8, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE2, datastore_create_resource(DATASTORE_TYPE_UINT32, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE3, datastore_create_resource(DATASTORE_TYPE_INT8, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE4, datastore_create_resource(DATASTORE_TYPE_INT32, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE5, datastore_create_resource(DATASTORE_TYPE_FLOAT, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE6, datastore_create_resource(DATASTORE_TYPE_DOUBLE, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE7, datastore_create_string_resource(256, 1)));

    char buffer[256] = {0};
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_bool(ds, RESOURCE0, 0, true));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE0, 0, buffer, 256));
    EXPECT_STREQ("true", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_bool(ds, RESOURCE0, 0, false));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE0, 0, buffer, 256));
    EXPECT_STREQ("false", buffer);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint8(ds, RESOURCE1, 0, 0));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE1, 0, buffer, 256));
    EXPECT_STREQ("0", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint8(ds, RESOURCE1, 0, 42));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE1, 0, buffer, 256));
    EXPECT_STREQ("42", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint8(ds, RESOURCE1, 0, 255));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE1, 0, buffer, 256));
    EXPECT_STREQ("255", buffer);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE2, 0, 0));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE2, 0, buffer, 256));
    EXPECT_STREQ("0", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE2, 0, 1234567));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE2, 0, buffer, 256));
    EXPECT_STREQ("1234567", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE2, 0, 4294967295));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE2, 0, buffer, 256));
    EXPECT_STREQ("4294967295", buffer);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int8(ds, RESOURCE3, 0, 0));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE3, 0, buffer, 256));
    EXPECT_STREQ("0", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int8(ds, RESOURCE3, 0, -17));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE3, 0, buffer, 256));
    EXPECT_STREQ("-17", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int8(ds, RESOURCE3, 0, 127));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE3, 0, buffer, 256));
    EXPECT_STREQ("127", buffer);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int32(ds, RESOURCE4, 0, 0));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE4, 0, buffer, 256));
    EXPECT_STREQ("0", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int32(ds, RESOURCE4, 0, -1234567));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE4, 0, buffer, 256));
    EXPECT_STREQ("-1234567", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_int32(ds, RESOURCE4, 0, 429496729));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE4, 0, buffer, 256));
    EXPECT_STREQ("429496729", buffer);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_float(ds, RESOURCE5, 0, 0));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE5, 0, buffer, 256));
    EXPECT_STREQ("0", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_float(ds, RESOURCE5, 0, -123.456));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE5, 0, buffer, 256));
    EXPECT_STREQ("-123.456", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_float(ds, RESOURCE5, 0, 12.345e23));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE5, 0, buffer, 256));
    EXPECT_STREQ("1.2345e+24", buffer);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_double(ds, RESOURCE6, 0, 0));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE6, 0, buffer, 256));
    EXPECT_STREQ("0", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_double(ds, RESOURCE6, 0, -123.456));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE6, 0, buffer, 256));
    EXPECT_STREQ("-123.456", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_double(ds, RESOURCE6, 0, 12.345e23));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE6, 0, buffer, 256));
    EXPECT_STREQ("1.2345e+24", buffer);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_string(ds, RESOURCE7, 0, ""));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE7, 0, buffer, 256));
    EXPECT_STREQ("", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_string(ds, RESOURCE7, 0, "-123.456"));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE7, 0, buffer, 256));
    EXPECT_STREQ("-123.456", buffer);
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_string(ds, RESOURCE7, 0, "12.345e+23"));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE7, 0, buffer, 256));
    EXPECT_STREQ("12.345e+23", buffer);

    datastore_free(&ds);
}

TEST(DatastoreTest, test_get_as_string_invalid) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE0, datastore_create_resource(DATASTORE_TYPE_BOOL, 1)));

    char buffer[256] = {0};
    EXPECT_EQ(DATASTORE_STATUS_ERROR_NULL_POINTER, datastore_get_as_string(NULL, RESOURCE0, 0, buffer, 256));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_NULL_POINTER, datastore_get_as_string(ds, RESOURCE0, 0, NULL, 256));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_ID, datastore_get_as_string(ds, RESOURCE1, 0, buffer, 256));
    EXPECT_EQ(DATASTORE_STATUS_ERROR_INVALID_INSTANCE, datastore_get_as_string(ds, RESOURCE0, 1, buffer, 256));
    datastore_free(&ds);
}

TEST(DatastoreTest, test_get_as_string_truncation) {
    datastore_t * ds = datastore_create();
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE0, datastore_create_resource(DATASTORE_TYPE_BOOL, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE2, datastore_create_resource(DATASTORE_TYPE_UINT32, 1)));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_add_resource(ds, RESOURCE7, datastore_create_string_resource(256, 1)));

    char buffer[3] = {0};
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_bool(ds, RESOURCE0, 0, true));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE0, 0, buffer, 3));
    EXPECT_STREQ("tr", buffer);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_bool(ds, RESOURCE0, 0, false));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE0, 0, buffer, 3));
    EXPECT_STREQ("fa", buffer);

    char buffer2[6] = {0};
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_uint32(ds, RESOURCE2, 0, 12345678));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE2, 0, buffer2, 6));
    EXPECT_STREQ("12345", buffer2);

    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_set_string(ds, RESOURCE7, 0, "1234567890"));
    EXPECT_EQ(DATASTORE_STATUS_OK, datastore_get_as_string(ds, RESOURCE7, 0, buffer2, 6));
    EXPECT_STREQ("12345", buffer2);

    datastore_free(&ds);
}
