add_library(NES_LIB
    cpu/cpu.h cpu/cpu.cpp)

add_executable(NES main.cpp)
target_link_libraries(NES NES_LIB)