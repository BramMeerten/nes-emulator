add_library(NES_LIB
    cpu/cpu.h cpu/cpu.cpp
    memory.h memory.cpp
    system.h
)

add_executable(NES main.cpp)
target_link_libraries(NES NES_LIB)