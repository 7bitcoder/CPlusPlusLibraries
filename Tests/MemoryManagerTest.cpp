#include <cstddef>
#include <format>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <thread>

#include "MemoryManager.hpp"

struct DestructionCntClass
{
    inline static int destructionCnt = 0;
    ~DestructionCntClass() { destructionCnt++; }
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

    void SetUp() override
    {
        // GTEST_SKIP() << "Skipping all tests for this fixture";
    }

    void TearDown() override {}

    ~MemoryManagerTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(MemoryManagerTest, AllocationTest)
{
    auto ptr = sd::make<DestructionCntClass>();

    EXPECT_NE(nullptr, ptr);
}

TEST_F(MemoryManagerTest, GarbageCollectTest)
{
    sd::make<DestructionCntClass>();
    sd::MemoryManager::instance().garbageCollect();

    EXPECT_EQ(1, DestructionCntClass::destructionCnt);
}

TEST_F(MemoryManagerTest, AllocationComplexTest)
{
    auto ptr = sd::make<ComplexClass>(1);

    EXPECT_NE(nullptr, ptr);
    EXPECT_EQ(1, ptr->a);
}

TEST_F(MemoryManagerTest, GBNotCollectingTest)
{
    sd::MemoryManager::instance();

    auto ptr = sd::make<DestructionCntClass>();
    auto p = &ptr;
    sd::MemoryManager::instance().garbageCollect();

    EXPECT_EQ(0, DestructionCntClass::destructionCnt);
}

TEST_F(MemoryManagerTest, BigGarbageCollectTest)
{
    sd::make<ComplexClass>(1);
    sd::make<ComplexClass>(1);
    sd::make<ComplexClass>(1);
    sd::make<ComplexClass>(1);
    sd::make<ComplexClass>(1);
    sd::make<ComplexClass>(1);

    sd::MemoryManager::instance().garbageCollect();

    EXPECT_EQ(6, DestructionCntClass::destructionCnt);
}

TEST_F(MemoryManagerTest, GarbageCollectCicleTest)
{
    {
        auto circle = sd::make<CirceClass>(nullptr);
        auto circle2 = sd::make<CirceClass>(circle);
        circle->a = circle2;
    }

    sd::MemoryManager::instance().garbageCollect();

    EXPECT_EQ(2, DestructionCntClass::destructionCnt);
}
