#include <gtest/gtest.h>
#include "datastore.h"

typedef enum {
	RESOURCE_INVALID = -1,
	RESOURCE0 = 0,
	RESOURCE1,
	RESOURCE2,
	RESOURCE3,
} resource_id;

TEST(DatastoreTest, malloc_and_free) {
    datastore_t * datastore = datastore_malloc();
    ASSERT_TRUE(NULL != datastore);
    datastore_free(&datastore);
    EXPECT_EQ(NULL, datastore);
}


// NEW API



TEST(Datastore2Test, create_and_free) {
	datastore2_t * ds = datastore2_create();
    ASSERT_TRUE(NULL != ds);
    datastore2_free(&ds);
    EXPECT_EQ(NULL, ds);
}

TEST(Datastore2Test, free_invalid) {
	datastore2_free(NULL);
	datastore2_t * ds = datastore2_create();
    datastore2_free(&ds);
    datastore2_free(&ds);
}

TEST(Datastore2Test, add_managed_scalar_uint32) {
	datastore2_t * ds = datastore2_create();
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));
    datastore2_free(&ds);
}

TEST(Datastore2Test, add_managed_scalar_string) {
    datastore2_t * ds = datastore2_create();
    const uint32_t STRING_LEN = 32;
    EXPECT_EQ(DATASTORE_OK, datastore2_add_string_resource(ds, RESOURCE0, 1, STRING_LEN));
    datastore2_free(&ds);
}

TEST(Datastore2Test, add_managed_tabular_uint32) {
	datastore2_t * ds = datastore2_create();
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1000));
    datastore2_free(&ds);
}

TEST(Datastore2Test, add_managed_tabular_string) {
    datastore2_t * ds = datastore2_create();
    const uint32_t STRING_LEN = 32;
    EXPECT_EQ(DATASTORE_OK, datastore2_add_string_resource(ds, RESOURCE0, 100, STRING_LEN));
    datastore2_free(&ds);
}

TEST(Datastore2Test, add_managed_scalar_uint32_to_null_datastore) {
	EXPECT_EQ(DATASTORE_ERROR_NULL_POINTER, datastore2_add_fixed_length_resource(NULL, RESOURCE0, DATASTORE_TYPE_UINT32, 1));
}

TEST(Datastore2Test, add_managed_with_invalid_id) {
	datastore2_t * ds = datastore2_create();
	EXPECT_EQ(DATASTORE_ERROR_INVALID_ID, datastore2_add_fixed_length_resource(ds, RESOURCE_INVALID, DATASTORE_TYPE_UINT32, 1));
	EXPECT_EQ(DATASTORE_ERROR_INVALID_ID, datastore2_add_fixed_length_resource(ds, -1000, DATASTORE_TYPE_UINT32, 1));
    datastore2_free(&ds);
}

TEST(Datastore2Test, add_managed_with_invalid_type) {
	datastore2_t * ds = datastore2_create();
	EXPECT_EQ(DATASTORE_ERROR_INVALID_TYPE, datastore2_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_LAST, 1));
    datastore2_free(&ds);
}

TEST(Datastore2Test, add_managed_with_invalid_number_of_instances) {
	datastore2_t * ds = datastore2_create();
	EXPECT_EQ(DATASTORE_ERROR_INVALID_INSTANCE, datastore2_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 0));
    datastore2_free(&ds);
}

TEST(Datastore2Test, add_managed_string_with_wrong_function) {
	datastore2_t * ds = datastore2_create();
	EXPECT_EQ(DATASTORE_ERROR_INVALID_TYPE, datastore2_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_STRING, 1));
    datastore2_free(&ds);
}

TEST(Datastore2Test, managed_scalar_uint32_defaults_to_zero) {
	datastore2_t * ds = datastore2_create();
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));

	uint32_t value = 42;
	EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, RESOURCE0, 0, &value));
	EXPECT_EQ(0, value);
    datastore2_free(&ds);
}

TEST(Datastore2Test, get_instance_of_scalar_out_of_range) {
	datastore2_t * ds = datastore2_create();
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));

	uint32_t value = 42;
	EXPECT_EQ(DATASTORE_ERROR_INVALID_INSTANCE, datastore2_get_uint32(ds, RESOURCE0, 1, &value));
	EXPECT_EQ(42, value);
	EXPECT_EQ(DATASTORE_ERROR_INVALID_INSTANCE, datastore2_get_uint32(ds, RESOURCE0, -1, &value));
	EXPECT_EQ(42, value);
	EXPECT_EQ(DATASTORE_ERROR_INVALID_INSTANCE, datastore2_get_uint32(ds, RESOURCE0, 100, &value));
	EXPECT_EQ(42, value);
    datastore2_free(&ds);
}

TEST(Datastore2Test, set_instance_of_scalar_out_of_range) {
    datastore2_t * ds = datastore2_create();
    EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));

    EXPECT_EQ(DATASTORE_ERROR_INVALID_INSTANCE, datastore2_set_uint32(ds, RESOURCE0, 1, 42));
    EXPECT_EQ(DATASTORE_ERROR_INVALID_INSTANCE, datastore2_set_uint32(ds, RESOURCE0, -1, 42));
    EXPECT_EQ(DATASTORE_ERROR_INVALID_INSTANCE, datastore2_set_uint32(ds, RESOURCE0, 100, 42));
    datastore2_free(&ds);
}

TEST(Datastore2Test, managed_scalar_uint32_set_and_get) {
	datastore2_t * ds = datastore2_create();
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));

	uint32_t value = 0;

	EXPECT_EQ(DATASTORE_OK, datastore2_set_uint32(ds, RESOURCE0, 0, 17));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, RESOURCE0, 0, &value));
	EXPECT_EQ(17, value);
    datastore2_free(&ds);
}

TEST(Datastore2Test, managed_scalar_uint32_set_and_get_multiple) {
	datastore2_t * ds = datastore2_create();
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));

	uint32_t value = 0;

	EXPECT_EQ(DATASTORE_OK, datastore2_set_uint32(ds, RESOURCE0, 0, UINT32_MAX));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, RESOURCE0, 0, &value));
	EXPECT_EQ(UINT32_MAX, value);

	EXPECT_EQ(DATASTORE_OK, datastore2_set_uint32(ds, RESOURCE0, 0, 0));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, RESOURCE0, 0, &value));
	EXPECT_EQ(0, value);

	EXPECT_EQ(DATASTORE_OK, datastore2_set_uint32(ds, RESOURCE0, 0, 1234567890));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, RESOURCE0, 0, &value));
	EXPECT_EQ(1234567890, value);
    datastore2_free(&ds);
}

TEST(Datastore2Test, managed_tabular_uint32_defaults_to_zero) {
	datastore2_t * ds = datastore2_create();
	const int NUM_INSTANCES = 1000;
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, NUM_INSTANCES));

	for (int i = 0; i < NUM_INSTANCES; ++i) {
		uint32_t value = 42;
		EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, RESOURCE0, i, &value));
		EXPECT_EQ(0, value);
	}
    datastore2_free(&ds);
}

TEST(Datastore2Test, get_instance_of_tabular_out_of_range) {
	datastore2_t * ds = datastore2_create();
	const int NUM_INSTANCES = 1000;
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, NUM_INSTANCES));

	uint32_t value = 42;
	EXPECT_EQ(DATASTORE_ERROR_INVALID_INSTANCE, datastore2_get_uint32(ds, RESOURCE0, NUM_INSTANCES, &value));
	EXPECT_EQ(42, value);
	EXPECT_EQ(DATASTORE_ERROR_INVALID_INSTANCE, datastore2_get_uint32(ds, RESOURCE0, NUM_INSTANCES + 1, &value));
	EXPECT_EQ(42, value);
	EXPECT_EQ(DATASTORE_ERROR_INVALID_INSTANCE, datastore2_get_uint32(ds, RESOURCE0, NUM_INSTANCES + 1000, &value));
	EXPECT_EQ(42, value);
    datastore2_free(&ds);
}

TEST(Datastore2Test, set_instance_of_tabular_out_of_range) {
    datastore2_t * ds = datastore2_create();
    const int NUM_INSTANCES = 1000;
    EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, NUM_INSTANCES));

    EXPECT_EQ(DATASTORE_ERROR_INVALID_INSTANCE, datastore2_set_uint32(ds, RESOURCE0, NUM_INSTANCES, 42));
    EXPECT_EQ(DATASTORE_ERROR_INVALID_INSTANCE, datastore2_set_uint32(ds, RESOURCE0, NUM_INSTANCES + 1, 42));
    EXPECT_EQ(DATASTORE_ERROR_INVALID_INSTANCE, datastore2_set_uint32(ds, RESOURCE0, NUM_INSTANCES + 1000, 42));
    EXPECT_EQ(DATASTORE_ERROR_INVALID_INSTANCE, datastore2_set_uint32(ds, RESOURCE0, -1, 42));
    datastore2_free(&ds);
}

TEST(Datastore2Test, get_unknown_resources) {
	datastore2_t * ds = datastore2_create();
	EXPECT_EQ(DATASTORE_ERROR_INVALID_ID, datastore2_get_bool(ds, RESOURCE0, 0, NULL));
	EXPECT_EQ(DATASTORE_ERROR_INVALID_ID, datastore2_get_uint32(ds, RESOURCE1, 4, NULL));
	EXPECT_EQ(DATASTORE_ERROR_INVALID_ID, datastore2_get_string(ds, RESOURCE2, 0, NULL));
    datastore2_free(&ds);
}

TEST(Datastore2Test, get_resource_with_wrong_function) {
	datastore2_t * ds = datastore2_create();
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, RESOURCE0, DATASTORE_TYPE_UINT32, 1));
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, RESOURCE1, DATASTORE_TYPE_BOOL, 1));
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, RESOURCE2, DATASTORE_TYPE_FLOAT, 1));

	EXPECT_EQ(DATASTORE_ERROR_INVALID_TYPE, datastore2_get_bool(ds, RESOURCE0, 0, NULL));
	EXPECT_EQ(DATASTORE_ERROR_INVALID_TYPE, datastore2_get_int32(ds, RESOURCE0, 0, NULL));
	EXPECT_EQ(DATASTORE_ERROR_INVALID_TYPE, datastore2_get_uint32(ds, RESOURCE1, 4, NULL));
	EXPECT_EQ(DATASTORE_ERROR_INVALID_TYPE, datastore2_get_uint8(ds, RESOURCE1, 4, NULL));
	EXPECT_EQ(DATASTORE_ERROR_INVALID_TYPE, datastore2_get_string(ds, RESOURCE2, 0, NULL));
	EXPECT_EQ(DATASTORE_ERROR_INVALID_TYPE, datastore2_get_double(ds, RESOURCE2, 0, NULL));
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_scalar_bool) {
	datastore2_t * ds = datastore2_create();
	const datastore2_resource_id_t ID = RESOURCE3;
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_BOOL, 1));
	bool value = false;
	EXPECT_EQ(DATASTORE_OK, datastore2_get_bool(ds, ID, 0, &value));
	EXPECT_FALSE(value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_bool(ds, ID, 0, true));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_bool(ds, ID, 0, &value));
	EXPECT_TRUE(value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_bool(ds, ID, 0, false));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_bool(ds, ID, 0, &value));
	EXPECT_FALSE(value);
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_scalar_uint8) {
	datastore2_t * ds = datastore2_create();
	const datastore2_resource_id_t ID = 99;
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_UINT8, 1));
	uint8_t value = 42;
	EXPECT_EQ(DATASTORE_OK, datastore2_get_uint8(ds, ID, 0, &value));
	EXPECT_EQ(0, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_uint8(ds, ID, 0, UINT8_MAX));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_uint8(ds, ID, 0, &value));
	EXPECT_EQ(UINT8_MAX, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_uint8(ds, ID, 0, 0));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_uint8(ds, ID, 0, &value));
	EXPECT_EQ(0, value);
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_scalar_uint32) {
	datastore2_t * ds = datastore2_create();
	const datastore2_resource_id_t ID = 1234;
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_UINT32, 1));
	uint32_t value = 42;
	EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, ID, 0, &value));
	EXPECT_EQ(0, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_uint32(ds, ID, 0, UINT32_MAX));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, ID, 0, &value));
	EXPECT_EQ(UINT32_MAX, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_uint32(ds, ID, 0, 0));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, ID, 0, &value));
	EXPECT_EQ(0, value);
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_scalar_int8) {
	datastore2_t * ds = datastore2_create();
	const datastore2_resource_id_t ID = 99;
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_INT8, 1));
	int8_t value = 42;
	EXPECT_EQ(DATASTORE_OK, datastore2_get_int8(ds, ID, 0, &value));
	EXPECT_EQ(0, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_int8(ds, ID, 0, INT8_MAX));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_int8(ds, ID, 0, &value));
	EXPECT_EQ(INT8_MAX, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_int8(ds, ID, 0, 0));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_int8(ds, ID, 0, &value));
	EXPECT_EQ(0, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_int8(ds, ID, 0, INT8_MIN));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_int8(ds, ID, 0, &value));
	EXPECT_EQ(INT8_MIN, value);
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_scalar_int32) {
	datastore2_t * ds = datastore2_create();
	const datastore2_resource_id_t ID = 1234;
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_INT32, 1));
	int32_t value = 42;
	EXPECT_EQ(DATASTORE_OK, datastore2_get_int32(ds, ID, 0, &value));
	EXPECT_EQ(0, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_int32(ds, ID, 0, INT32_MAX));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_int32(ds, ID, 0, &value));
	EXPECT_EQ(INT32_MAX, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_int32(ds, ID, 0, 0));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_int32(ds, ID, 0, &value));
	EXPECT_EQ(0, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_int32(ds, ID, 0, INT32_MIN));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_int32(ds, ID, 0, &value));
	EXPECT_EQ(INT32_MIN, value);
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_scalar_float) {
	datastore2_t * ds = datastore2_create();
	const datastore2_resource_id_t ID = 1;
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_FLOAT, 1));
	float value = 42.7f;
	EXPECT_EQ(DATASTORE_OK, datastore2_get_float(ds, ID, 0, &value));
	EXPECT_EQ(0.0f, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_float(ds, ID, 0, FLT_MAX));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_float(ds, ID, 0, &value));
	EXPECT_EQ(FLT_MAX, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_float(ds, ID, 0, 0.0f));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_float(ds, ID, 0, &value));
	EXPECT_EQ(0.0f, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_float(ds, ID, 0, FLT_MIN));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_float(ds, ID, 0, &value));
	EXPECT_EQ(FLT_MIN, value);
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_scalar_double) {
	datastore2_t * ds = datastore2_create();
	const datastore2_resource_id_t ID = 42;
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_DOUBLE, 1));
	double value = 42.7;
	EXPECT_EQ(DATASTORE_OK, datastore2_get_double(ds, ID, 0, &value));
	EXPECT_EQ(0.0f, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_double(ds, ID, 0, DBL_MAX));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_double(ds, ID, 0, &value));
	EXPECT_EQ(DBL_MAX, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_double(ds, ID, 0, 0.0));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_double(ds, ID, 0, &value));
	EXPECT_EQ(0.0, value);
	EXPECT_EQ(DATASTORE_OK, datastore2_set_double(ds, ID, 0, DBL_MIN));
	EXPECT_EQ(DATASTORE_OK, datastore2_get_double(ds, ID, 0, &value));
	EXPECT_EQ(DBL_MIN, value);
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_scalar_string) {
    datastore2_t * ds = datastore2_create();
    const datastore2_resource_id_t ID = 42;
    const uint32_t STRING_LEN = 8;  // inc. 1 for null terminator
    EXPECT_EQ(DATASTORE_OK, datastore2_add_string_resource(ds, ID, 1, STRING_LEN));
    char value[STRING_LEN] = "";
    EXPECT_EQ(DATASTORE_OK, datastore2_get_string(ds, ID, 0, value));
    EXPECT_STREQ("", value);
    EXPECT_EQ(DATASTORE_OK, datastore2_set_string(ds, ID, 0, "abcdefg"));
    EXPECT_EQ(DATASTORE_OK, datastore2_get_string(ds, ID, 0, value));
    EXPECT_STREQ("abcdefg", value);
    EXPECT_EQ(DATASTORE_OK, datastore2_set_string(ds, ID, 0, ""));
    EXPECT_EQ(DATASTORE_OK, datastore2_get_string(ds, ID, 0, value));
    EXPECT_STREQ("", value);
    EXPECT_EQ(DATASTORE_OK, datastore2_set_string(ds, ID, 0, "1234567"));
    EXPECT_EQ(DATASTORE_OK, datastore2_get_string(ds, ID, 0, value));
    EXPECT_STREQ("1234567", value);
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_scalar_string_exceeds_allocated_length) {
    datastore2_t * ds = datastore2_create();
    const datastore2_resource_id_t ID = 4;
    const uint32_t STRING_LEN = 8;  // inc. 1 for null terminator
    char value[STRING_LEN] = "";
    EXPECT_EQ(DATASTORE_OK, datastore2_add_string_resource(ds, ID, 1, STRING_LEN));
    EXPECT_EQ(DATASTORE_ERROR_TOO_LARGE, datastore2_set_string(ds, ID, 0, "abcdefghijkl"));
    EXPECT_STREQ("", value);
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_tabular_bool) {
	datastore2_t * ds = datastore2_create();
	const datastore2_resource_id_t ID = 199;
	const uint32_t NUM_INSTANCES = 200;
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_BOOL, NUM_INSTANCES));

	// check defaults
	for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
		bool value = false;
		EXPECT_EQ(DATASTORE_OK, datastore2_get_bool(ds, ID, i, &value));
		EXPECT_FALSE(value);
	}

	// set all to predictable values
	for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
		EXPECT_EQ(DATASTORE_OK, datastore2_set_bool(ds, ID, i, (i / 3) % 2 ? true : false));
	}

	for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
		bool value = false;
		EXPECT_EQ(DATASTORE_OK, datastore2_get_bool(ds, ID, i, &value));
		EXPECT_EQ((i / 3) % 2 ? true : false, value);
	}
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_tabular_uint8) {
	datastore2_t * ds = datastore2_create();
	const datastore2_resource_id_t ID = 55;
	const uint32_t NUM_INSTANCES = 1000;
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_UINT8, NUM_INSTANCES));

	// check defaults
	for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
		uint8_t value = 0;
		EXPECT_EQ(DATASTORE_OK, datastore2_get_uint8(ds, ID, i, &value));
		EXPECT_FALSE(value);
	}

	// set all to predictable values
	for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
		EXPECT_EQ(DATASTORE_OK, datastore2_set_uint8(ds, ID, i, i * i + 3 * i - 7));
	}

	for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
		uint8_t value = 0;
		EXPECT_EQ(DATASTORE_OK, datastore2_get_uint8(ds, ID, i, &value));
		EXPECT_EQ((uint8_t)(i * i + 3 * i - 7), value);
	}
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_tabular_uint32) {
	datastore2_t * ds = datastore2_create();
	const datastore2_resource_id_t ID = 55;
	const uint32_t NUM_INSTANCES = 1000;
	EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_UINT32, NUM_INSTANCES));

	// check defaults
	for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
		uint32_t value = 0;
		EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, ID, i, &value));
		EXPECT_FALSE(value);
	}

	// set all to predictable values
	for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
		EXPECT_EQ(DATASTORE_OK, datastore2_set_uint32(ds, ID, i, 3 * i * i * i + 2 * i * i + 5 * i - 16));
	}

	for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
		uint32_t value = 0;
		EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, ID, i, &value));
		EXPECT_EQ((uint32_t)(3 * i * i * i + 2 * i * i + 5 * i - 16), value);
	}
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_tabular_int8) {
    datastore2_t * ds = datastore2_create();
    const datastore2_resource_id_t ID = 55;
    const int32_t NUM_INSTANCES = 1000;
    EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_INT8, NUM_INSTANCES));

    // check defaults
    for (int32_t i = 0; i < NUM_INSTANCES; ++i) {
        int8_t value = 0;
        EXPECT_EQ(DATASTORE_OK, datastore2_get_int8(ds, ID, i, &value));
        EXPECT_FALSE(value);
    }

    // set all to predictable values
    for (int32_t i = 0; i < NUM_INSTANCES; ++i) {
        EXPECT_EQ(DATASTORE_OK, datastore2_set_int8(ds, ID, i, i * i + 3 * i - 7));
    }

    for (int32_t i = 0; i < NUM_INSTANCES; ++i) {
        int8_t value = 0;
        EXPECT_EQ(DATASTORE_OK, datastore2_get_int8(ds, ID, i, &value));
        EXPECT_EQ((int8_t)(i * i + 3 * i - 7), value);
    }
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_tabular_int32) {
    datastore2_t * ds = datastore2_create();
    const datastore2_resource_id_t ID = 55;
    const int32_t NUM_INSTANCES = 1000;
    EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_INT32, NUM_INSTANCES));

    // check defaults
    for (int32_t i = 0; i < NUM_INSTANCES; ++i) {
        int32_t value = 0;
        EXPECT_EQ(DATASTORE_OK, datastore2_get_int32(ds, ID, i, &value));
        EXPECT_FALSE(value);
    }

    // set all to predictable values
    for (int32_t i = 0; i < NUM_INSTANCES; ++i) {
        EXPECT_EQ(DATASTORE_OK, datastore2_set_int32(ds, ID, i, 3 * i * i * i + 2 * i * i + 5 * i - 16));
    }

    for (int32_t i = 0; i < NUM_INSTANCES; ++i) {
        int32_t value = 0;
        EXPECT_EQ(DATASTORE_OK, datastore2_get_int32(ds, ID, i, &value));
        EXPECT_EQ((int32_t)(3 * i * i * i + 2 * i * i + 5 * i - 16), value);
    }
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_tabular_float) {
    datastore2_t * ds = datastore2_create();
    const datastore2_resource_id_t ID = 199;
    const uint32_t NUM_INSTANCES = 19;
    EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_FLOAT, NUM_INSTANCES));

    // check defaults
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        float value = 0.0f;
        EXPECT_EQ(DATASTORE_OK, datastore2_get_float(ds, ID, i, &value));
        EXPECT_EQ(0.0f, value);
    }

    // set all to predictable values
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        EXPECT_EQ(DATASTORE_OK, datastore2_set_float(ds, ID, i, 0.91f * i * i + 145.67f * i - 953.213f));
    }

    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        float value = 0.0f;
        EXPECT_EQ(DATASTORE_OK, datastore2_get_float(ds, ID, i, &value));
        EXPECT_EQ(0.91f * i * i + 145.67f * i - 953.213f, value);
    }
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_tabular_double) {
    datastore2_t * ds = datastore2_create();
    const datastore2_resource_id_t ID = 199;
    const uint32_t NUM_INSTANCES = 521;
    EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_DOUBLE, NUM_INSTANCES));

    // check defaults
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        double value = 0.0;
        EXPECT_EQ(DATASTORE_OK, datastore2_get_double(ds, ID, i, &value));
        EXPECT_EQ(0.0, value);
    }

    // set all to predictable values
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        EXPECT_EQ(DATASTORE_OK, datastore2_set_double(ds, ID, i, 67.91f * i * i + 135.67f * i - 953.213f));
    }

    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        double value = 0.0;
        EXPECT_EQ(DATASTORE_OK, datastore2_get_double(ds, ID, i, &value));
        EXPECT_EQ(67.91f * i * i + 135.67f * i - 953.213f, value);
    }
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_tabular_string) {
    datastore2_t * ds = datastore2_create();
    const datastore2_resource_id_t ID = 5432;
    const uint32_t NUM_INSTANCES = 21;
    const uint32_t STRING_LEN = 8;  // inc. 1 for null terminator
    char value[STRING_LEN] = "";
    EXPECT_EQ(DATASTORE_OK, datastore2_add_string_resource(ds, ID, NUM_INSTANCES, STRING_LEN));

    // check defaults
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        memset(value, 0, STRING_LEN);
        EXPECT_EQ(DATASTORE_OK, datastore2_get_string(ds, ID, i, value));
        EXPECT_STREQ("", value);
    }

    // set all to predictable values
    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        char s[STRING_LEN] = "";
        snprintf(s, STRING_LEN, "abc-%03d", i);
        EXPECT_EQ(DATASTORE_OK, datastore2_set_string(ds, ID, i, s));
    }

    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        memset(value, 0, STRING_LEN);
        EXPECT_EQ(DATASTORE_OK, datastore2_get_string(ds, ID, i, value));
        char s[STRING_LEN] = "";
        snprintf(s, STRING_LEN, "abc-%03d", i);
        EXPECT_STREQ(s, value);
    }
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_tabular_string_exceeds_allocated_length) {
    datastore2_t * ds = datastore2_create();
    const datastore2_resource_id_t ID = 43;
    const uint32_t NUM_INSTANCES = 54;
    const uint32_t STRING_LEN = 64;  // inc. 1 for null terminator
    char value[STRING_LEN] = "";
    EXPECT_EQ(DATASTORE_OK, datastore2_add_string_resource(ds, ID, NUM_INSTANCES, STRING_LEN));

    for (uint32_t i = 0; i < NUM_INSTANCES; ++i) {
        char * s = (char *)malloc(STRING_LEN + 1 + i);
        memset(s, 'A', STRING_LEN + i);
        s[STRING_LEN + i] = '\0';
        EXPECT_EQ(DATASTORE_ERROR_TOO_LARGE, datastore2_set_string(ds, ID, i, s));
        free(s);
    }
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_create_and_add_resources) {
    datastore2_t * ds = datastore2_create();
    EXPECT_EQ(DATASTORE_OK, datastore2_add_resource(ds, RESOURCE0, datastore2_create_resource(DATASTORE_TYPE_UINT8, 1)));
    EXPECT_EQ(DATASTORE_OK, datastore2_add_resource(ds, RESOURCE1, datastore2_create_resource(DATASTORE_TYPE_INT32, 7)));
    EXPECT_EQ(DATASTORE_OK, datastore2_add_resource(ds, RESOURCE2, datastore2_create_string_resource(32, 1)));
    EXPECT_EQ(DATASTORE_OK, datastore2_add_resource(ds, RESOURCE3, datastore2_create_string_resource(8, 8)));
    datastore2_free(&ds);
}

TEST(Datastore2Test, test_add_static_resources) {
    datastore2_t * ds = datastore2_create();
    struct st {
        uint32_t d0;
        int8_t d1[3];
        char s0[32];
        char s1[3][4];
    } st = { 42, {-7, 0, 5}, "hello", { "abc", "def", "ghi" } };
    EXPECT_EQ(DATASTORE_OK, datastore2_add_resource(ds, RESOURCE0, (datastore_resource_t){.data=&st.d0, .size=sizeof(st.d0), .type=DATASTORE_TYPE_UINT32, .num_instances=1 }));
    EXPECT_EQ(DATASTORE_OK, datastore2_add_resource(ds, RESOURCE1, (datastore_resource_t){.data=&st.d1, .size=sizeof(st.d1[0]), .type=DATASTORE_TYPE_INT8, .num_instances=3 }));
    EXPECT_EQ(DATASTORE_OK, datastore2_add_resource(ds, RESOURCE2, (datastore_resource_t){.data=st.s0, .size=sizeof(st.s0), .type=DATASTORE_TYPE_STRING, .num_instances=1 }));
    EXPECT_EQ(DATASTORE_OK, datastore2_add_resource(ds, RESOURCE3, (datastore_resource_t){.data=st.s1, .size=sizeof(st.s1[0]), .type=DATASTORE_TYPE_STRING, .num_instances=3 }));

    uint32_t d0 = 0;
    EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, RESOURCE0, 0, &d0));
    EXPECT_EQ(st.d0, d0);

    int8_t d1 = 0;
    EXPECT_EQ(DATASTORE_OK, datastore2_get_int8(ds, RESOURCE1, 0, &d1));
    EXPECT_EQ(st.d1[0], d1);
    EXPECT_EQ(DATASTORE_OK, datastore2_get_int8(ds, RESOURCE1, 1, &d1));
    EXPECT_EQ(st.d1[1], d1);
    EXPECT_EQ(DATASTORE_OK, datastore2_get_int8(ds, RESOURCE1, 2, &d1));
    EXPECT_EQ(st.d1[2], d1);

    datastore2_free(&ds);
}

namespace detail {
    struct CallbackParameterRecord {
        CallbackParameterRecord() : datastore(nullptr), resource_id(-1), instance_id(-1), context(nullptr) {}
        datastore2_t * datastore;
        datastore2_resource_id_t resource_id;
        datastore2_instance_id_t instance_id;
        void * context;
    };

    struct CallbackRecord {
        CallbackRecord() : counter(0), last() {}
        int counter;
        CallbackParameterRecord last;
    };

    static void callback(datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, void * context) {
        std::cout << "callback: " << "datastore " << datastore << ", id " << id << ", instance " << instance << ", context " << context << std::endl;
        CallbackRecord * record = static_cast<CallbackRecord *>(context);
        ++record->counter;
        record->last.datastore = datastore;
        record->last.resource_id = id;
        record->last.instance_id = instance;
        record->last.context = context;
    }
}

TEST(Datastore2Test, test_set_callback) {
    datastore2_t * ds = datastore2_create();
    const datastore2_resource_id_t ID = 422;

    EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_UINT32, 10));

    detail::CallbackRecord record;
    EXPECT_EQ(DATASTORE_OK, datastore2_add_set_callback(ds, ID, detail::callback, &record));

    EXPECT_EQ(DATASTORE_OK, datastore2_set_uint32(ds, ID, 0, 42));
    uint32_t value = 0;
    EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, ID, 0, &value));

    EXPECT_EQ(1, record.counter);
    EXPECT_EQ(ds, record.last.datastore);
    EXPECT_EQ(ID, record.last.resource_id);
    EXPECT_EQ(0, record.last.instance_id);
    EXPECT_EQ(&record, record.last.context);
    EXPECT_EQ(42, value);

    EXPECT_EQ(DATASTORE_OK, datastore2_set_uint32(ds, ID, 5, 17));
    EXPECT_EQ(2, record.counter);
    EXPECT_EQ(ds, record.last.datastore);
    EXPECT_EQ(ID, record.last.resource_id);
    EXPECT_EQ(5, record.last.instance_id);
    EXPECT_EQ(&record, record.last.context);

    EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, ID, 5, &value));
    EXPECT_EQ(17, value);

    datastore2_free(&ds);
}

namespace detail {

    struct SharedCallbackRecord {
        SharedCallbackRecord(int * shared) : shared(shared) {}
        CallbackRecord record;
        int * shared;
    };

    static void chained_callback1(datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, void * context) {
        std::cout << "chained_callback1: " << "datastore " << datastore << ", id " << id << ", instance " << instance << ", context " << context << std::endl;
        SharedCallbackRecord * record = static_cast<SharedCallbackRecord *>(context);
        callback(datastore, id, instance, &record->record);
        *record->shared += 7;  // add 7
    }

    static void chained_callback2(datastore2_t * datastore, datastore2_resource_id_t id, datastore2_instance_id_t instance, void * context) {
        std::cout << "chained_callback2: " << "datastore " << datastore << ", id " << id << ", instance " << instance << ", context " << context << std::endl;
        SharedCallbackRecord * record = static_cast<SharedCallbackRecord *>(context);
        callback(datastore, id, instance, &record->record);
        *record->shared *= 2;  // double
    }

}

TEST(Datastore2Test, test_set_callback_chain) {
    // add multiple callbacks, check they are all invoked in the correct order
    datastore2_t * ds = datastore2_create();
    const datastore2_resource_id_t ID = 19;

    int shared = 19;
    detail::SharedCallbackRecord record1(&shared);
    detail::SharedCallbackRecord record2(&shared);

    EXPECT_EQ(DATASTORE_OK, datastore2_add_fixed_length_resource(ds, ID, DATASTORE_TYPE_UINT32, 10));

    EXPECT_EQ(DATASTORE_OK, datastore2_add_set_callback(ds, ID, detail::chained_callback1, &record1));
    EXPECT_EQ(DATASTORE_OK, datastore2_add_set_callback(ds, ID, detail::chained_callback2, &record2));

    EXPECT_EQ(DATASTORE_OK, datastore2_set_uint32(ds, ID, 0, 42));

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

    uint32_t value = 0;
    EXPECT_EQ(DATASTORE_OK, datastore2_get_uint32(ds, ID, 0, &value));
    EXPECT_EQ(42, value);

    // shared_callback1 then shared_callback2 => +7 *2
    EXPECT_EQ((19 + 7) * 2, shared);

    datastore2_free(&ds);
}

TEST(Datastore2Test, DISABLED_test_get_from_set_callback) {
    // TODO: test that another value can be read from datastore by the callback
}

TEST(Datastore2Test, DISABLED_test_set_from_set_callback) {
    // TODO: test that another value can be written to the datastore by the callback
}
