#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include "DependencyInjector.hpp"

struct SimpleClass
{

    int gg = 100;

    SimpleClass() {}
};

struct OneRefClass
{
    int ff = 100;
    SimpleClass *_a;

    OneRefClass(SimpleClass *a) { _a = a; }
};

struct IInheranceClass
{
    virtual int get() = 0;
    virtual SimpleClass *getSimple() = 0;
    virtual const OneRefClass *getOneRef() = 0;
    virtual ~IInheranceClass() {}
};

struct InheranceClass final : public IInheranceClass
{
    int ff = 100;
    SimpleClass *_a;
    const OneRefClass &_b;

    InheranceClass(SimpleClass *a, const OneRefClass *b) : _b(*b) { _a = a; }

    int get() final { return ff; }
    SimpleClass *getSimple() { return _a; }
    const OneRefClass *getOneRef() { return &_b; }
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

TEST_F(DependencyInjector, AddServiceTest) { EXPECT_NO_THROW((di.addSingeleton<SimpleClass, SimpleClass>())); }

TEST_F(DependencyInjector, AddMultipleServicesTest)
{
    di.addSingeleton<SimpleClass, SimpleClass>();
    di.addSingeleton<OneRefClass, OneRefClass>();
    di.addSingeleton<IInheranceClass, InheranceClass>();

    di.build();

    EXPECT_TRUE(di.getPtr<SimpleClass>());
    EXPECT_TRUE(di.getPtr<OneRefClass>());
    EXPECT_TRUE(di.getPtr<IInheranceClass>());
}

TEST_F(DependencyInjector, AddMultipleServicesTestRef)
{
    di.addSingeleton<SimpleClass, SimpleClass>();
    di.addSingeleton<OneRefClass, OneRefClass>();
    di.addSingeleton<IInheranceClass, InheranceClass>();

    di.build();

    EXPECT_NO_THROW(di.getRef<SimpleClass>());
    EXPECT_NO_THROW(di.getRef<OneRefClass>());
    EXPECT_NO_THROW(di.getRef<IInheranceClass>());
}

TEST_F(DependencyInjector, SingeletonInstance)
{
    di.addSingeleton<SimpleClass, SimpleClass>();
    di.addSingeleton<OneRefClass, OneRefClass>();
    di.addSingeleton<IInheranceClass, InheranceClass>();

    di.build();

    auto simple = di.getPtr<SimpleClass>();
    auto ref = di.getPtr<OneRefClass>();
    auto inheritance = di.getPtr<IInheranceClass>();
    EXPECT_TRUE(inheritance);
    EXPECT_TRUE(inheritance->get());
    EXPECT_EQ(simple, inheritance->getSimple());
    EXPECT_EQ(ref, inheritance->getOneRef());
}

TEST_F(DependencyInjector, BuildFail)
{
    di.addSingeleton<IInheranceClass, InheranceClass>();

    EXPECT_THROW(di.build(), std::runtime_error);
}