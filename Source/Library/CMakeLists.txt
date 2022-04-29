file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_library(SandboxLib ${SOURCES})

target_include_directories(SandboxLib PUBLIC
  h
)