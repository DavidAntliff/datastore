#include <gtest/gtest.h>
#include "datastore.h"

TEST(DatastoreTest, malloc_and_free) {
    datastore_t * datastore = datastore_malloc();
    ASSERT_TRUE(NULL != datastore);
    datastore_free(&datastore);
    EXPECT_EQ(NULL, datastore);
}
