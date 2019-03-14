add_executable(aggregated aggregated.cpp)
target_link_libraries(aggregated polo::polo GTest::Main)
add_test(NAME polo.boosting.aggregated COMMAND aggregated)

add_executable(none none.cpp)
target_link_libraries(none polo::polo GTest::Main)
add_test(NAME polo.boosting.none COMMAND none)
