add_executable(constant constant.cpp)
target_link_libraries(constant
  PRIVATE
    polo::polo
    GTest::Main
)
add_test(NAME polo.step.constant COMMAND constant)

add_executable(decreasing decreasing.cpp)
target_link_libraries(decreasing
  PRIVATE
    polo::polo
    GTest::Main
)
add_test(NAME polo.step.decreasing COMMAND decreasing)
