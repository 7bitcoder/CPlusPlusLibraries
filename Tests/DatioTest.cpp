#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include "Date.hpp"

using namespace sd;

class DatioTest : public ::testing::Test
{
  protected:
    DatioTest() {}

    void SetUp() override {}

    void TearDown() override {}

    ~DatioTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(DatioTest, ExampleTest)
{
    Date date = Date::now();

    auto day = date.toString("{:L%F %T %Z}");
    EXPECT_EQ(day, "123");
}