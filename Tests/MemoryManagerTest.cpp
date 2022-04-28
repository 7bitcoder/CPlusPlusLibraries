#include <format>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <thread>

#include "MemoryManager.hpp "

struct DestructionCntClass : public sd::Object
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
    std::shared_ptr<DestructionCntClass> a = nullptr;
    CirceClass(std::shared_ptr<DestructionCntClass> a) { this->a = a; }
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
    auto ptr = sd::MemoryManager::instance().create<DestructionCntClass>();

    EXPECT_EQ(2, ptr.use_count());
}

TEST_F(MemoryManagerTest, ToStringTest)
{
    auto ptr = sd::MemoryManager::instance().create<DestructionCntClass>();

    auto className = ptr->toString();

    EXPECT_TRUE(className.starts_with("[Object: "));
}

TEST_F(MemoryManagerTest, HashCodeTest)
{
    auto ptr = sd::MemoryManager::instance().create<DestructionCntClass>();

    auto code = ptr->getHashCode();

    EXPECT_TRUE(code > 0);
}

TEST_F(MemoryManagerTest, GarbageCollectTest)
{
    sd::MemoryManager::instance().create<DestructionCntClass>();
    sd::MemoryManager::instance().garbageCollect();

    EXPECT_EQ(1, DestructionCntClass::destructionCnt);
}

TEST_F(MemoryManagerTest, AllocationComplexTest)
{
    auto ptr = sd::MemoryManager::instance().create<ComplexClass>(1);

    EXPECT_EQ(1, ptr->a);
    EXPECT_EQ(2, ptr.use_count());
}

TEST_F(MemoryManagerTest, BigGarbageCollectTest)
{
    sd::MemoryManager::instance().create<ComplexClass>(1);
    sd::MemoryManager::instance().create<ComplexClass>(1);
    sd::MemoryManager::instance().create<ComplexClass>(1);
    sd::MemoryManager::instance().create<ComplexClass>(1);
    sd::MemoryManager::instance().create<ComplexClass>(1);
    sd::MemoryManager::instance().create<ComplexClass>(1);

    sd::MemoryManager::instance().garbageCollect();

    EXPECT_EQ(6, DestructionCntClass::destructionCnt);
}

TEST_F(MemoryManagerTest, GarbageCollectCicleTest)
{
    {
        auto circle = sd::MemoryManager::instance().create<CirceClass>(nullptr);
        auto circle2 = sd::MemoryManager::instance().create<CirceClass>(circle);
        circle->a = circle2;
    }

    sd::MemoryManager::instance().garbageCollect();

    // todo implement circle detection and fix it to 2
    EXPECT_EQ(0, DestructionCntClass::destructionCnt);
}
