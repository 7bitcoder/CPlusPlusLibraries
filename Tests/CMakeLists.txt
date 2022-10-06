# file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(Test
    RunTests.cpp
    ListTest.cpp
    MapTest.cpp
    MemoryManagerTest.cpp
    CacheTest.cpp
    ArrayTest.cpp
    VectorTest.cpp
)

target_link_libraries(Test
    SandboxLib
    CONAN_PKG::gtest
)

target_include_directories(Test PRIVATE
    h
)

gtest_discover_tests(Test Tests PROPERTIES TIMEOUT 10)