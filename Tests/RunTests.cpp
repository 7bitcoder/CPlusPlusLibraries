#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
    // Run a specific test only
    testing::GTEST_FLAG(filter) = "MemoryManagerTest.*";

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}