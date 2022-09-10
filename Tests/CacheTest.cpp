#include "Cache.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>

using namespace std::string_literals;

class CacheTest : public ::testing::Test
{
  public:
    struct ExampleClass
    {
        const std::string name = "example";
    };

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
    auto classItem = sd::CacheItem<CacheTest::ExampleClass>::Make("class", CacheTest::ExampleClass{});
    auto boolItem2 = sd::CacheItem<bool>::Make("bool", true);

    EXPECT_TRUE(cache.Add(std::move(intItem)));
    EXPECT_TRUE(cache.Add(std::move(stringItem)));
    EXPECT_TRUE(cache.Add(std::move(boolItem)));
    EXPECT_TRUE(cache.Add(std::move(classItem)));
    EXPECT_FALSE(cache.Add(std::move(boolItem2)));
}

TEST_F(CacheTest, AddDirectTest)
{
    sd::Cache cache;

    EXPECT_TRUE(cache.Add("int", 12));
    EXPECT_TRUE(cache.Add("string", "hello"s));
    EXPECT_TRUE(cache.Add("bool", true));
    EXPECT_TRUE(cache.Add("class", CacheTest::ExampleClass{}));
    EXPECT_FALSE(cache.Add("bool", false));
}

TEST_F(CacheTest, CountTest)
{
    sd::Cache cache;

    cache.Add("int", 12);
    cache.Add("string", "hello"s);
    cache.Add("bool", true);
    cache.Add("class", CacheTest::ExampleClass{});
    cache.Add("bool", false);

    EXPECT_EQ(cache.Count(), 4);
}

TEST_F(CacheTest, ContainsTest)
{
    sd::Cache cache;

    cache.Add("int", 12);
    cache.Add("string", "hello"s);
    cache.Add("bool", true);
    cache.Add("class", CacheTest::ExampleClass{});
    cache.Add("bool", false);

    EXPECT_TRUE(cache.Contains("int"));
    EXPECT_TRUE(cache.Contains("class"));
}

TEST_F(CacheTest, SetTest)
{
    sd::Cache cache;

    EXPECT_TRUE(cache.Add("int", 12));
    EXPECT_TRUE(cache.Set("int", 14));
    EXPECT_FALSE(cache.Set("string", 14));

    EXPECT_EQ(*cache.Get<int>("int"), 14);
}

TEST_F(CacheTest, SetDualTest)
{
    sd::Cache cache;

    EXPECT_TRUE(cache.Add("dual", 12));
    EXPECT_TRUE(cache.Set("dual", CacheTest::ExampleClass{}));
    EXPECT_FALSE(cache.Get<int>("dual"));
    EXPECT_EQ(cache.Get<CacheTest::ExampleClass>("dual")->name, "example"s);
}

TEST_F(CacheTest, GetTest)
{
    sd::Cache cache;

    cache.Add("int", 12);
    cache.Add("string", "hello"s);
    cache.Add("class", CacheTest::ExampleClass{});
    cache.Add("bool", false);

    EXPECT_EQ(*cache.Get<int>("int"), 12);
    EXPECT_EQ(*cache.Get<std::string>("string"), "hello");
    EXPECT_EQ(*cache.Get<bool>("bool"), false);
    EXPECT_EQ(cache.Get<CacheTest::ExampleClass>("class")->name, "example"s);
    EXPECT_FALSE(cache.Get<int>("bool"));
}

TEST_F(CacheTest, GetItemTest)
{
    sd::Cache cache;

    cache.Add("int", 12);
    cache.Add("string", "hello"s);
    cache.Add("class", CacheTest::ExampleClass{});
    cache.Add("bool", false);

    EXPECT_EQ(*cache.GetItem<int>("int")->GetTypedValue(), 12);
    EXPECT_EQ(*cache.GetItem<std::string>("string")->GetTypedValue(), "hello");
    EXPECT_EQ(*cache.GetItem<bool>("bool")->GetTypedValue(), false);
    EXPECT_EQ(cache.GetItem<CacheTest::ExampleClass>("class")->GetTypedValue()->name, "example"s);
    EXPECT_FALSE(cache.GetItem<int>("bool"));
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
        [&](const int *oldValue, const int *newValue) { combinedResult = *oldValue - *newValue; });

    EXPECT_TRUE(cache.Add("int", 7, std::move(policy)));
    EXPECT_TRUE(cache.Set("int", 3));
    EXPECT_EQ(combinedResult, 4);
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

TEST_F(CacheTest, GeneralUsage)
{
    struct A
    {
        const std::string name = "lily";
    };

    sd::Cache cache;

    cache.Add("dual", 1234);
    auto res = cache.Get<int>("dual");
    auto updated = cache.Set("dual", true);
    auto afterUpdate = cache.Get<bool>("dual");
    auto isEqual = *afterUpdate == true;

    A a;
    auto updated2 = cache.Set("dual", std::move(a));
    auto afterUpdate2 = cache.Get<A>("dual");
    auto isEqual2 = afterUpdate2->name == "lily";
    cache.Contains("dual");
    auto g = cache.Count();
    cache.Remove("dual");
    auto gh = cache.Count();
}

TEST_F(CacheTest, WrapperAddTest)
{
    sd::CacheWrapper cache;

    EXPECT_TRUE(cache.Add("int", 12));
    EXPECT_TRUE(cache.Add("string", "hello"s));
    EXPECT_TRUE(cache.Add("bool", true));
    EXPECT_TRUE(cache.Add("class", CacheTest::ExampleClass{}));
    EXPECT_FALSE(cache.Add("bool", false));
}

TEST_F(CacheTest, WrapperCountTest)
{
    sd::CacheWrapper cache;

    cache.Add("int", 12);
    cache.Add("string", "hello"s);
    cache.Add("bool", true);
    cache.Add("class", CacheTest::ExampleClass{});
    cache.Add("bool", false);

    EXPECT_EQ(cache.Count(), 4);
}

TEST_F(CacheTest, WrapperContainsTest)
{
    sd::CacheWrapper cache;

    cache.Add("int", 12);
    cache.Add("string", "hello"s);
    cache.Add("bool", true);
    cache.Add("class", CacheTest::ExampleClass{});
    cache.Add("bool", false);

    EXPECT_TRUE(cache.Contains<int>("int"));
    EXPECT_TRUE(cache.Contains<CacheTest::ExampleClass>("class"));
}

TEST_F(CacheTest, WrapperSetTest)
{
    sd::CacheWrapper cache;

    EXPECT_TRUE(cache.Add("int", 12));
    EXPECT_TRUE(cache.Set("int", 14));
    EXPECT_FALSE(cache.Set("string", 14));

    EXPECT_EQ(*cache.Get<int>("int"), 14);
}

TEST_F(CacheTest, WrapperSetDualTest)
{
    sd::CacheWrapper cache;

    EXPECT_TRUE(cache.Add("dual", 12));
    EXPECT_TRUE(cache.Set("dual", 19));
    EXPECT_FALSE(cache.Set("dual", CacheTest::ExampleClass{}));
    EXPECT_EQ(*cache.Get<int>("dual"), 19);
    EXPECT_FALSE(cache.Get<CacheTest::ExampleClass>("dual"));
}

TEST_F(CacheTest, WrapperGetTest)
{
    sd::CacheWrapper cache;

    cache.Add("int", 12);
    cache.Add("string", "hello"s);
    cache.Add("class", CacheTest::ExampleClass{});
    cache.Add("bool", false);

    EXPECT_EQ(*cache.Get<int>("int"), 12);
    EXPECT_EQ(*cache.Get<std::string>("string"), "hello");
    EXPECT_EQ(*cache.Get<bool>("bool"), false);
    EXPECT_EQ(cache.Get<CacheTest::ExampleClass>("class")->name, "example"s);
    EXPECT_FALSE(cache.Get<int>("bool"));
}

TEST_F(CacheTest, WrapperRemoveTest)
{
    sd::CacheWrapper cache;

    cache.Add("int", 12);
    cache.Add("string", "hello"s);
    cache.Add("bool", false);

    EXPECT_TRUE(cache.Remove<int>("int"));
    EXPECT_FALSE(cache.Remove<int>("nonExisting"));
    EXPECT_FALSE(cache.Contains<int>("int"));
}

TEST_F(CacheTest, WrapperAddPolicyTest)
{
    sd::CacheWrapper cache;

    std::unique_ptr<sd::CachePolicy<int>> policy(new sd::CachePolicy<int>(
        [](const int *oldValue, const int *newValue) { int result = *oldValue + *newValue; }, [](const int *value) {}));

    EXPECT_TRUE(cache.Add("int", 12, std::move(policy)));
}

TEST_F(CacheTest, WrapperOnUpdateCallbackTest)
{
    sd::CacheWrapper cache;

    int combinedResult = 0;

    auto policy = sd::CachePolicy<int>::Make();
    policy->SetOnUpdateCallback(
        [&](const int *oldValue, const int *newValue) { combinedResult = *oldValue - *newValue; });

    EXPECT_TRUE(cache.Add("int", 7, std::move(policy)));
    EXPECT_TRUE(cache.Set("int", 3));
    EXPECT_EQ(combinedResult, 4);
}

TEST_F(CacheTest, WrapperOnRemoveCallbackTest)
{
    sd::CacheWrapper cache;

    bool removedCalled = false;

    auto policy = sd::CachePolicy<int>::Make([&](const int *oldValue, const int *newValue) {},
                                             [&](const int *oldValue) { removedCalled = true; });

    EXPECT_TRUE(cache.Add("int", 7, std::move(policy)));
    EXPECT_TRUE(cache.Remove<int>("int"));
    EXPECT_TRUE(removedCalled);
}

TEST_F(CacheTest, WrapperGeneralUsage)
{
    struct A
    {
        const std::string name = "lily";
    };

    sd::CacheWrapper cache;

    cache.Add("dual", 1234);
    cache.Add("dual", true);
    cache.Add("dual", "hello"s);
    cache.Add("dual", A{});
    auto intRes = cache.Get<int>("dual");
    auto boolRes = cache.Get<bool>("dual");
    auto stringRes = cache.Get<std::string>("dual");
    auto classRes = cache.Get<A>("dual");
    auto isEqual = *boolRes == true;
    auto isEqualString = *stringRes == "hello"s;

    auto updated = cache.Set("dual", false);
    boolRes = cache.Get<bool>("dual");
    auto contains = cache.Contains<int>("dual");
    auto countBefore = cache.Count();
    cache.Remove<int>("dual");
    auto countAfter = cache.Count();
}
