add_library(NES_LIB
    cpu/cpu.h cpu/cpu.cpp
    cpu/addressing_mode.cpp
    bus.h bus.cpp
    rom.cpp
)

add_executable(NES main.cpp)
target_link_libraries(NES NES_LIB)