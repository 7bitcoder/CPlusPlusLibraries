#include <cstddef>
#include <format>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <thread>
#include <unordered_set>

#include "MemoryManager.hpp"

struct ExampleClass
{
    ExampleClass *ptr = nullptr;
    std::unordered_set<ExampleClass *> &destructorCnt;

    ExampleClass(std::unordered_set<ExampleClass *> &destructorCnt, ExampleClass *ptr = nullptr)
        : ptr(ptr), destructorCnt(destructorCnt)
    {
    }

    ~ExampleClass() { destructorCnt.insert(this); }
};

class MemoryManagerTest : public ::testing::Test
{
  protected:
    std::unordered_set<ExampleClass *> destructorSet;

    static void SetUpTestSuite() {}

    MemoryManagerTest() {}

    void SetUp() override { destructorSet.clear(); }

    void TearDown() override { sd::MemoryManager::instance().garbageCollect(); }

    ~MemoryManagerTest() {}

    static void TearDownTestSuite() {}

    bool wasDestructed(std::vector<ExampleClass *> ptrs)
    {
        for (auto ptr : ptrs)
        {
            if (destructorSet.contains(ptr))
            {
                return true;
            }
        }
        return false;
    }
    int destructionCnt() { return destructorSet.size(); }

    ExampleClass *make(ExampleClass *ptr = nullptr) { return sd::make<ExampleClass>(destructorSet, ptr); }
};

TEST_F(MemoryManagerTest, ManagerShouldAllocateObject)
{
    auto ptr = make();

    EXPECT_NE(nullptr, ptr);
    EXPECT_EQ(nullptr, ptr->ptr);
}

TEST_F(MemoryManagerTest, ManagerShouldCollectObjects)
{
    make();
    make();
    make();
    make();
    make();
    make();
    make();
    make();
    make();
    make();
    make();
    make();
    make();
    make();

    sd::MemoryManager::instance().garbageCollect();

    EXPECT_LE(1, destructionCnt());
}

TEST_F(MemoryManagerTest, ManagerShouldNotCollectObject)
{
    auto ptr1 = make();
    auto ptr2 = make();
    auto ptr3 = make();
    auto ptr4 = make();

    sd::MemoryManager::instance().garbageCollect();

    EXPECT_FALSE(wasDestructed({ptr1, ptr2, ptr3, ptr4}));
}

TEST_F(MemoryManagerTest, ManagerShouldCollectObjectsWithCircleReferences)
{
    {
        auto circle = make(nullptr);
        auto circle2 = make(circle);
        circle->ptr = circle2;

        circle = make(nullptr);
        circle2 = make(circle);
        circle->ptr = circle2;

        circle = make(nullptr);
        circle2 = make(circle);
        circle->ptr = circle2;

        circle = make(nullptr);
        circle2 = make(circle);
        circle->ptr = circle2;

        circle = make(nullptr);
        circle2 = make(circle);
        circle->ptr = circle2;
    }

    sd::MemoryManager::instance().garbageCollect();

    EXPECT_LE(1, destructionCnt());
}

TEST_F(MemoryManagerTest, ManagerShouldCollectSomeObjects)
{
    auto circle = make(nullptr);
    auto circle2 = make(circle);
    circle->ptr = circle2;
    {

        auto toRemove = make(nullptr);
        auto toRemove2 = make(toRemove);
        toRemove->ptr = toRemove2;

        toRemove = make(nullptr);
        toRemove2 = make(toRemove);
        toRemove->ptr = toRemove2;

        toRemove = make(nullptr);
        toRemove2 = make(toRemove);
        toRemove->ptr = toRemove2;
    }

    sd::MemoryManager::instance().garbageCollect();

    EXPECT_FALSE(wasDestructed({circle, circle2}));
    EXPECT_LE(1, destructionCnt());
}

TEST_F(MemoryManagerTest, ManagerShouldNotCollectAutomaticallySomeObjects)
{
    auto limit = 500 * 1024 / sizeof(ExampleClass);
    for (int i = 0; i < limit; i++) // allocate ~0.5 MB
    {
        make();
    }

    EXPECT_EQ(0, destructionCnt());
}

TEST_F(MemoryManagerTest, ManagerShouldCollectAutomaticallySomeObjects)
{
    auto limit = 1500 * 1024 / sizeof(ExampleClass);
    for (int i = 0; i < limit; i++) // allocate ~1.5 MB
    {
        make();
    }

    EXPECT_LE(1, destructionCnt());
}

TEST_F(MemoryManagerTest, ManagersShouldWorkInSeparateThreads)
{
    auto mainThreadOb = make();

    std::unordered_set<ExampleClass *> destructorR1;
    std::unordered_set<ExampleClass *> destructorR2;

    std::jthread r1{[&]() {
        auto limit = 1500 * 1024 / sizeof(ExampleClass);
        for (int i = 0; i < limit; i++)
        {
            sd::make<ExampleClass>(destructorR1);
        }
        EXPECT_LE(1, destructorR1.size());
    }};

    std::jthread r2{[&]() {
        auto limit = 1500 * 1024 / sizeof(ExampleClass);
        for (int i = 0; i < limit; i++)
        {
            sd::make<ExampleClass>(destructorR2);
        }
        EXPECT_LE(1, destructorR1.size());
    }};

    r1.join();
    r2.join();
    EXPECT_FALSE(wasDestructed({mainThreadOb}));
    EXPECT_EQ(0, destructionCnt());
}