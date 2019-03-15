add_executable(anatomy anatomy.cpp)
target_link_libraries(anatomy polo::polo)
add_test(NAME polo.examples.getting.anatomy COMMAND anatomy)
set_tests_properties(polo.examples.getting.anatomy PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 0.030*;Optimizer: [-4.57*,4.61*,4.61*,]."
)
