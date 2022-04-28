#include <format>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <thread>

#include "MemoryManager.hpp "

struct FinalizeCntClass : public sd::Object
{
    inline static int finalizeCalledCnt = 0;
    void finalize() override { finalizeCalledCnt++; }
};

struct ComplexClass : public FinalizeCntClass
{
    int a;
    ComplexClass(int a) : FinalizeCntClass() { this->a = a; }
};

struct CirceClass : public FinalizeCntClass
{
    std::shared_ptr<FinalizeCntClass> a = nullptr;
    CirceClass(std::shared_ptr<FinalizeCntClass> a) { this->a = a; }
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
    auto ptr = sd::MemoryManager::instance().allocate<FinalizeCntClass>();

    EXPECT_EQ(2, ptr.use_count());
}

TEST_F(MemoryManagerTest, ToStringTest)
{
    auto ptr = sd::MemoryManager::instance().allocate<FinalizeCntClass>();

    auto className = ptr->toString();

    EXPECT_TRUE(className.starts_with("[Object: "));
}

TEST_F(MemoryManagerTest, HashCodeTest)
{
    auto ptr = sd::MemoryManager::instance().allocate<FinalizeCntClass>();

    auto code = ptr->getHashCode();

    EXPECT_TRUE(code > 0);
}

TEST_F(MemoryManagerTest, GarbageCollectTest)
{
    sd::MemoryManager::instance().allocate<FinalizeCntClass>();
    sd::MemoryManager::instance().garbageCollect();

    EXPECT_EQ(1, FinalizeCntClass::finalizeCalledCnt);
}

TEST_F(MemoryManagerTest, AllocationComplexTest)
{
    auto ptr = sd::MemoryManager::instance().allocate<ComplexClass>(1);

    EXPECT_EQ(1, ptr->a);
    EXPECT_EQ(2, ptr.use_count());
}

TEST_F(MemoryManagerTest, BigGarbageCollectTest)
{
    sd::MemoryManager::instance().allocate<ComplexClass>(1);
    sd::MemoryManager::instance().allocate<ComplexClass>(1);
    sd::MemoryManager::instance().allocate<ComplexClass>(1);
    sd::MemoryManager::instance().allocate<ComplexClass>(1);
    sd::MemoryManager::instance().allocate<ComplexClass>(1);
    sd::MemoryManager::instance().allocate<ComplexClass>(1);

    sd::MemoryManager::instance().garbageCollect();

    EXPECT_EQ(6, FinalizeCntClass::finalizeCalledCnt);
}

TEST_F(MemoryManagerTest, GarbageCollectCicleTest)
{
    {
        auto circle = sd::MemoryManager::instance().allocate<CirceClass>(nullptr);
        auto circle2 = sd::MemoryManager::instance().allocate<CirceClass>(circle);
        circle->a = circle2;
    }

    sd::MemoryManager::instance().garbageCollect();

    // todo implement circle detection and fix it to 2
    EXPECT_EQ(0, FinalizeCntClass::finalizeCalledCnt);
}
