#include <gtest/gtest.h>
#include <iostream>
#include <thread>

class Template : public ::testing::Test
{
  protected:
    Template() {}

    void SetUp() override
    {
        // GTEST_SKIP() << "Skipping all tests for this fixture";
    }

    void TearDown() override {}

    ~Template() {}

    static void TearDownTestSuite() {}
};

TEST_F(Template, ExampleTest) { EXPECT_TRUE(true); }
