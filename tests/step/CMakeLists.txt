add_executable(constant constant.cpp)
target_link_libraries(constant polo::polo GTest::Main)
add_test(NAME polo.step.constant COMMAND constant)

add_executable(decreasing decreasing.cpp)
target_link_libraries(decreasing polo::polo GTest::Main)
add_test(NAME polo.step.decreasing COMMAND decreasing)
