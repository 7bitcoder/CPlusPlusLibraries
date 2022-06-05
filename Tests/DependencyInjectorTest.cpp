#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include "DependencyInjector.hpp"

struct TestA
{
    static TestA *constructor() { return new TestA(); };
};

struct TestB
{
    static TestB *constructor(TestA *a) { return new TestB(); };
};

class DependencyInjector : public ::testing::Test
{
  protected:
    sd::DependencyInjector di;

    static void SetUpTestSuite() {}

    DependencyInjector() {}

    void SetUp() override {}

    void TearDown() override {}

    ~DependencyInjector() {}

    static void TearDownTestSuite() {}
};

TEST_F(DependencyInjector, ExampleTest)
{
    di.addSingeleton<TestA, TestA>();
    di.addSingeleton<TestB, TestB>();
}
