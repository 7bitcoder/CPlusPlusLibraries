
add_subdirectory(Library)

add_executable(Sandbox
  main.cpp
)

target_link_libraries(Sandbox 
  SandboxLib
)