#file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_library(SandboxLib 
  LinkedList.cpp
  Map.cpp
  MemoryManager.cpp
  DependencyInjector.cpp
)

target_include_directories(SandboxLib PUBLIC
  h
)