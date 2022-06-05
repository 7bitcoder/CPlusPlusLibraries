#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include "DependencyInjector.hpp"

#define Injectable() static
struct TestA
{
    static TestA *constructor() { return new TestA(); };

    int gg = 100;

    Injectable() TestA() {}
};

struct TestB
{
    static TestB *constructor(TestA *a) { return new TestB(); };

    int ff = 100;
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

    di.build();

    TestA *a = di.get<TestA>();

    TestB *b = di.get<TestB>();
    int g = 0;
}
