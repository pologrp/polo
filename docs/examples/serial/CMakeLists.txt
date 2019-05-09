add_executable(logistic-momentum logistic.cpp)
target_compile_definitions(logistic-momentum PUBLIC MOMENTUM)
target_link_libraries(logistic-momentum polo::polo)
add_test(NAME polo.examples.serial.logistic.momentum COMMAND logistic-momentum)
set_tests_properties(polo.examples.serial.logistic.momentum PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 226.197719"
)

add_executable(logistic-nesterov logistic.cpp)
target_compile_definitions(logistic-nesterov PUBLIC NESTEROV)
target_link_libraries(logistic-nesterov polo::polo)
add_test(NAME polo.examples.serial.logistic.nesterov COMMAND logistic-nesterov)
set_tests_properties(polo.examples.serial.logistic.nesterov PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 225.614595"
)

add_executable(logistic-adagrad logistic.cpp)
target_compile_definitions(logistic-adagrad PUBLIC ADAGRAD)
target_link_libraries(logistic-adagrad polo::polo)
add_test(NAME polo.examples.serial.logistic.adagrad COMMAND logistic-adagrad)
set_tests_properties(polo.examples.serial.logistic.adagrad PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 226.557627"
)

add_executable(logistic-adam logistic.cpp)
target_compile_definitions(logistic-adam PUBLIC ADAM)
target_link_libraries(logistic-adam polo::polo)
add_test(NAME polo.examples.serial.logistic.adam COMMAND logistic-adam)
set_tests_properties(polo.examples.serial.logistic.adam PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 241.091232"
)
