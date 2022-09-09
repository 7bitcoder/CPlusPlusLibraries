#include "Cache.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>

using namespace std::string_literals;

class CacheTest : public ::testing::Test
{
  protected:
    static void SetUpTestSuite() {}

    CacheTest() {}

    void SetUp() override {}

    void TearDown() override {}

    ~CacheTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(CacheTest, AddTest)
{
    sd::Cache cache;

    auto intItem = sd::CacheItem<int>::Make("int", 12);
    auto stringItem = sd::CacheItem<std::string>::Make("string", "hello"s);
    auto boolItem = sd::CacheItem<bool>::Make("bool", false);
    auto boolItem2 = sd::CacheItem<bool>::Make("bool", true);

    EXPECT_TRUE(cache.Add(std::move(intItem)));
    EXPECT_TRUE(cache.Add(std::move(stringItem)));
    EXPECT_TRUE(cache.Add(std::move(boolItem)));
    EXPECT_FALSE(cache.Add(std::move(boolItem2)));
}

TEST_F(CacheTest, AddDirectTest)
{
    sd::Cache cache;

    EXPECT_TRUE(cache.Add("int", 12));
    EXPECT_TRUE(cache.Add("string", "hello"s));
    EXPECT_TRUE(cache.Add("bool", true));
    EXPECT_FALSE(cache.Add("bool", false));
}

TEST_F(CacheTest, CountTest)
{
    sd::Cache cache;

    cache.Add("int", 12);
    cache.Add("string", "hello"s);
    cache.Add("bool", true);
    cache.Add("bool", false);

    EXPECT_EQ(cache.Count(), 3);
}

TEST_F(CacheTest, ContainsTest)
{
    sd::Cache cache;

    cache.Add("int", 12);
    cache.Add("string", "hello"s);
    cache.Add("bool", true);
    cache.Add("bool", false);

    EXPECT_TRUE(cache.Contains("int"));
}

TEST_F(CacheTest, SetTest)
{
    sd::Cache cache;

    EXPECT_TRUE(cache.Add("int", 12));
    EXPECT_TRUE(cache.Set("int", 14));
    EXPECT_FALSE(cache.Set("string", 14));
    EXPECT_EQ(*cache.Get<int>("int"), 14);
}

TEST_F(CacheTest, GetTest)
{
    sd::Cache cache;

    cache.Add("int", 12);
    cache.Add("string", "hello"s);
    cache.Add("bool", false);

    EXPECT_EQ(*cache.Get<int>("int"), 12);
    EXPECT_EQ(*cache.Get<std::string>("string"), "hello");
    EXPECT_EQ(*cache.Get<bool>("bool"), false);
}

TEST_F(CacheTest, RemoveTest)
{
    sd::Cache cache;

    cache.Add("int", 12);
    cache.Add("string", "hello"s);
    cache.Add("bool", false);

    EXPECT_TRUE(cache.Remove("int"));
    EXPECT_FALSE(cache.Remove("nonExisting"));
    EXPECT_FALSE(cache.Contains("int"));
}

TEST_F(CacheTest, AddPolicyTest)
{
    sd::Cache cache;

    std::unique_ptr<sd::CachePolicy<int>> policy(new sd::CachePolicy<int>(
        [](const int *oldValue, const int *newValue) { int result = *oldValue + *newValue; }, [](const int *value) {}));

    EXPECT_TRUE(cache.Add("int", 12, std::move(policy)));
}

TEST_F(CacheTest, OnUpdateCallbackTest)
{
    sd::Cache cache;

    int combinedResult = 0;

    auto policy = sd::CachePolicy<int>::Make();
    policy->SetOnUpdateCallback(
        [&](const int *oldValue, const int *newValue) { combinedResult = *oldValue + *newValue; });

    EXPECT_TRUE(cache.Add("int", 7, std::move(policy)));
    EXPECT_TRUE(cache.Set("int", 3));
    EXPECT_EQ(combinedResult, 10);
}

TEST_F(CacheTest, OnRemoveCallbackTest)
{
    sd::Cache cache;

    bool removedCalled = false;

    auto policy = sd::CachePolicy<int>::Make([&](const int *oldValue, const int *newValue) {},
                                             [&](const int *oldValue) { removedCalled = true; });

    EXPECT_TRUE(cache.Add("int", 7, std::move(policy)));
    EXPECT_TRUE(cache.Remove("int"));
    EXPECT_TRUE(removedCalled);
}