include_directories (${NES_SOURCE_DIR}/src)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

file(GLOB SRCS cpu_instructions_test.cpp cpu_addressing_mode_test.cpp memory_test.cpp)
add_executable( NES_TEST ${SRCS} )
target_link_libraries( NES_TEST NES_LIB gtest_main )

include(GoogleTest)
gtest_discover_tests(NES_TEST)