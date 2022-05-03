#include <cstddef>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "MemoryManager.hpp"

struct ExampleClass
{
    ExampleClass *ptr = nullptr;
    std::vector<ExampleClass *> &collectedObjects;

    ExampleClass(std::vector<ExampleClass *> &vec, ExampleClass *ptr = nullptr) : ptr(ptr), collectedObjects(vec) {}
    ~ExampleClass() { collectedObjects.push_back(this); }
};

class MemoryManagerTest : public ::testing::Test
{
  protected:
    static std::vector<ExampleClass *> &getCollectedObjects()
    {
        static std::vector<ExampleClass *> ob;
        return ob;
    }

    void SetUp() override { getCollectedObjects().clear(); }

    void TearDown() override { sd::MemoryManager::instance().garbageCollect(); }

    bool wasCollected(std::vector<ExampleClass *> ptrs)
    {
        for (auto ptr : ptrs)
        {
            auto &v = getCollectedObjects();
            if (std::find(v.begin(), v.end(), ptr) != v.end())
            {
                return true;
            }
        }
        return false;
    }

    int collectedCnt() { return getCollectedObjects().size(); }

    ExampleClass *make(ExampleClass *ptr = nullptr) { return sd::make<ExampleClass>(getCollectedObjects(), ptr); }
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

    EXPECT_LE(1, collectedCnt());
}

TEST_F(MemoryManagerTest, ManagerShouldNotCollectObject)
{
    auto ptr1 = make();
    auto ptr2 = make();
    auto ptr3 = make();
    auto ptr4 = make();

    sd::MemoryManager::instance().garbageCollect();

    EXPECT_FALSE(wasCollected({ptr1, ptr2, ptr3, ptr4}));
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

    EXPECT_LE(1, collectedCnt());
}

TEST_F(MemoryManagerTest, ManagerShouldCollectSomeObjects)
{
    auto circle = make(nullptr);
    circle->ptr = make(circle);
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

    EXPECT_FALSE(wasCollected({circle, circle->ptr}));
    EXPECT_LE(1, collectedCnt());
}

TEST_F(MemoryManagerTest, ManagerShouldNotCollectInnerObjects)
{
    auto circle = make(nullptr);
    circle->ptr = make(nullptr);
    circle->ptr->ptr = make(nullptr);
    circle->ptr->ptr->ptr = make(nullptr);
    circle->ptr->ptr->ptr->ptr = make(nullptr);

    sd::MemoryManager::instance().garbageCollect();

    EXPECT_FALSE(
        wasCollected({circle, circle->ptr, circle->ptr->ptr, circle->ptr->ptr->ptr, circle->ptr->ptr->ptr->ptr}));
}

TEST_F(MemoryManagerTest, ManagerShouldNotCollectAutomaticallySomeObjects)
{
    auto limit = 500 * 1024 / sizeof(ExampleClass);
    for (int i = 0; i < limit; i++) // allocate ~0.5 MB
    {
        make();
    }

    EXPECT_EQ(0, collectedCnt());
}

TEST_F(MemoryManagerTest, ManagerShouldCollectAutomaticallySomeObjects)
{
    auto limit = 1500 * 1024 / sizeof(ExampleClass);
    for (int i = 0; i < limit; i++) // allocate ~1.5 MB
    {
        make();
    }

    EXPECT_LE(1, collectedCnt());
}

TEST_F(MemoryManagerTest, ManagersShouldWorkInSeparateThreads)
{
    auto mainThreadOb = make();

    auto limit = 1500 * 1024 / sizeof(ExampleClass);

    auto runner = [limit](std::vector<ExampleClass *> &destructor) {
        for (int i = 0; i < limit; i++)
        {
            sd::make<ExampleClass>(destructor);
        }
        EXPECT_LE(1, destructor.size());
    };

    std::vector<ExampleClass *> destructorR1;
    std::vector<ExampleClass *> destructorR2;

    std::thread r1{runner, std::ref(destructorR1)};
    std::thread r2{runner, std::ref(destructorR2)};

    r1.join();
    r2.join();
    EXPECT_FALSE(wasCollected({mainThreadOb}));
    EXPECT_EQ(limit, destructorR1.size());
    EXPECT_EQ(limit, destructorR2.size());
    EXPECT_EQ(0, collectedCnt());
}