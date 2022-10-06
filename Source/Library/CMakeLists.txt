# file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_library(SandboxLib
  LinkedList.cpp
  Map.cpp
  Cache.cpp
  MemoryManager.cpp
  Array.c
  Vector.c
)

target_include_directories(SandboxLib PUBLIC
  h
)