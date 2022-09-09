# file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(Test
    RunTests.cpp
    ListTest.cpp
    MapTest.cpp
    MemoryManagerTest.cpp
    DependencyInjectorTest.cpp
    CacheTest.cpp
)

target_link_libraries(Test
    SandboxLib
    CONAN_PKG::gtest
)

target_include_directories(Test PRIVATE
    h
)

gtest_discover_tests(Test Tests PROPERTIES TIMEOUT 10)