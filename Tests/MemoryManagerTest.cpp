#include <cstddef>
#include <format>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <thread>
#include <unordered_set>

#include "MemoryManager.hpp"

struct DestructionCntClass
{
    static std::unordered_set<DestructionCntClass *> &getDestructed()
    {
        static thread_local std::unordered_set<DestructionCntClass *> ob;
        return ob;
    }
    static bool wasDestructed(std::vector<DestructionCntClass *> ptrs)
    {
        for (auto ptr : ptrs)
        {
            if (getDestructed().contains(ptr))
            {
                return true;
            }
        }
        return false;
    }
    static int destructionCnt() { return getDestructed().size(); }

    ~DestructionCntClass() { getDestructed().insert(this); }
};

struct ComplexClass : public DestructionCntClass
{
    int a;
    ComplexClass(int a) : DestructionCntClass() { this->a = a; }
};

struct CirceClass : public DestructionCntClass
{
    DestructionCntClass *a = nullptr;
    CirceClass(DestructionCntClass *a) { this->a = a; }
};

class MemoryManagerTest : public ::testing::Test
{
  protected:
    static void SetUpTestSuite() {}

    MemoryManagerTest() {}

    void SetUp() override { DestructionCntClass::getDestructed().clear(); }

    void TearDown() override { sd::MemoryManager::instance().garbageCollect(); }

    ~MemoryManagerTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(MemoryManagerTest, ManagerShouldAllocateObject)
{
    auto ptr = sd::make<DestructionCntClass>();

    EXPECT_NE(nullptr, ptr);
}

TEST_F(MemoryManagerTest, ManagerShouldCollectObjects)
{
    sd::make<DestructionCntClass>();
    sd::make<DestructionCntClass>();
    sd::make<DestructionCntClass>();
    sd::make<DestructionCntClass>();
    sd::make<DestructionCntClass>();
    sd::make<DestructionCntClass>();
    sd::make<DestructionCntClass>();
    sd::make<DestructionCntClass>();
    sd::make<DestructionCntClass>();
    sd::make<DestructionCntClass>();
    sd::make<DestructionCntClass>();
    sd::make<DestructionCntClass>();
    sd::make<DestructionCntClass>();
    sd::make<DestructionCntClass>();

    sd::MemoryManager::instance().garbageCollect();

    EXPECT_LE(1, DestructionCntClass::destructionCnt());
}

TEST_F(MemoryManagerTest, ManagerShouldAllocateComplexObject)
{
    auto ptr = sd::make<ComplexClass>(1);

    EXPECT_NE(nullptr, ptr);
    EXPECT_EQ(1, ptr->a);
}

TEST_F(MemoryManagerTest, ManagerShouldNotCollectComplexObject)
{
    auto ptr1 = sd::make<ComplexClass>(1);
    auto ptr2 = sd::make<ComplexClass>(1);
    auto ptr3 = sd::make<ComplexClass>(1);
    auto ptr4 = sd::make<ComplexClass>(1);

    sd::MemoryManager::instance().garbageCollect();

    EXPECT_FALSE(DestructionCntClass::wasDestructed({ptr1, ptr2, ptr3, ptr4}));
}

TEST_F(MemoryManagerTest, ManagerShouldCollectObjectsWithCircleReferences)
{
    {
        auto circle = sd::make<CirceClass>(nullptr);
        auto circle2 = sd::make<CirceClass>(circle);
        circle->a = circle2;

        circle = sd::make<CirceClass>(nullptr);
        circle2 = sd::make<CirceClass>(circle);
        circle->a = circle2;

        circle = sd::make<CirceClass>(nullptr);
        circle2 = sd::make<CirceClass>(circle);
        circle->a = circle2;

        circle = sd::make<CirceClass>(nullptr);
        circle2 = sd::make<CirceClass>(circle);
        circle->a = circle2;

        circle = sd::make<CirceClass>(nullptr);
        circle2 = sd::make<CirceClass>(circle);
        circle->a = circle2;
    }

    sd::MemoryManager::instance().garbageCollect();

    EXPECT_LE(1, DestructionCntClass::destructionCnt());
}

TEST_F(MemoryManagerTest, ManagerShouldCollectSomeObjects)
{
    auto circle = sd::make<CirceClass>(nullptr);
    auto circle2 = sd::make<CirceClass>(circle);
    circle->a = circle2;
    {

        auto toRemove = sd::make<CirceClass>(nullptr);
        auto toRemove2 = sd::make<CirceClass>(toRemove);
        toRemove->a = toRemove2;

        toRemove = sd::make<CirceClass>(nullptr);
        toRemove2 = sd::make<CirceClass>(toRemove);
        toRemove->a = toRemove2;

        toRemove = sd::make<CirceClass>(nullptr);
        toRemove2 = sd::make<CirceClass>(toRemove);
        toRemove->a = toRemove2;
    }

    sd::MemoryManager::instance().garbageCollect();

    EXPECT_FALSE(DestructionCntClass::wasDestructed({circle, circle2}));
    EXPECT_LE(1, DestructionCntClass::destructionCnt());
}

TEST_F(MemoryManagerTest, ManagerShouldNotCollectAutomaticallySomeObjects)
{
    auto limit = 500 * 1024 / sizeof(ComplexClass);
    for (int i = 0; i < limit; i++) // allocate ~0.5 MB
    {
        sd::make<ComplexClass>(12);
    }

    EXPECT_EQ(0, DestructionCntClass::destructionCnt());
}

TEST_F(MemoryManagerTest, ManagerShouldCollectAutomaticallySomeObjects)
{
    auto limit = 1500 * 1024 / sizeof(ComplexClass);
    for (int i = 0; i < limit; i++) // allocate ~1.5 MB
    {
        sd::make<ComplexClass>(12);
    }

    EXPECT_LE(1, DestructionCntClass::destructionCnt());
}

TEST_F(MemoryManagerTest, ManagersShouldWorkInSeparateThreads)
{
    auto mainThreadOb = sd::make<ComplexClass>(12);

    std::jthread r1{[]() {
        auto limit = 1500 * 1024 / sizeof(ComplexClass);
        for (int i = 0; i < limit; i++)
        {
            sd::make<ComplexClass>(12);
        }
        EXPECT_LE(1, DestructionCntClass::destructionCnt());
        sd::MemoryManager::instance().wipeout();
    }};

    std::jthread r2{[]() {
        auto limit = 1500 * 1024 / sizeof(ComplexClass);
        for (int i = 0; i < limit; i++)
        {
            sd::make<ComplexClass>(12);
        }
        EXPECT_LE(1, DestructionCntClass::destructionCnt());
        sd::MemoryManager::instance().wipeout();
    }};

    r1.join();
    r2.join();
    EXPECT_FALSE(DestructionCntClass::wasDestructed({mainThreadOb}));
    EXPECT_EQ(0, DestructionCntClass::destructionCnt());
}