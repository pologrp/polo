add_executable(logistic-momentum logistic.cpp)
target_compile_definitions(logistic-momentum PRIVATE MOMENTUM)
target_link_libraries(logistic-momentum PRIVATE polo::polo)
add_test(NAME polo.examples.serial.logistic.momentum COMMAND logistic-momentum)
set_tests_properties(polo.examples.serial.logistic.momentum PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 226.197719"
)

add_executable(logistic-nesterov logistic.cpp)
target_compile_definitions(logistic-nesterov PRIVATE NESTEROV)
target_link_libraries(logistic-nesterov PRIVATE polo::polo)
add_test(NAME polo.examples.serial.logistic.nesterov COMMAND logistic-nesterov)
set_tests_properties(polo.examples.serial.logistic.nesterov PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 225.614595"
)

add_executable(logistic-adagrad logistic.cpp)
target_compile_definitions(logistic-adagrad PRIVATE ADAGRAD)
target_link_libraries(logistic-adagrad PRIVATE polo::polo)
add_test(NAME polo.examples.serial.logistic.adagrad COMMAND logistic-adagrad)
set_tests_properties(polo.examples.serial.logistic.adagrad PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 226.557627"
)

add_executable(logistic-adam logistic.cpp)
target_compile_definitions(logistic-adam PRIVATE ADAM)
target_link_libraries(logistic-adam PRIVATE polo::polo)
add_test(NAME polo.examples.serial.logistic.adam COMMAND logistic-adam)
set_tests_properties(polo.examples.serial.logistic.adam PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 241.091232"
)

add_executable(logistic-l1-momentum logistic-l1.cpp)
target_compile_definitions(logistic-l1-momentum PRIVATE MOMENTUM)
target_link_libraries(logistic-l1-momentum PRIVATE polo::polo)
add_test(NAME polo.examples.serial.logistic.l1.momentum COMMAND logistic-l1-momentum)
set_tests_properties(polo.examples.serial.logistic.l1.momentum PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 226.197719"
)

add_executable(logistic-l1-nesterov logistic-l1.cpp)
target_compile_definitions(logistic-l1-nesterov PRIVATE NESTEROV)
target_link_libraries(logistic-l1-nesterov PRIVATE polo::polo)
add_test(NAME polo.examples.serial.logistic.l1.nesterov COMMAND logistic-l1-nesterov)
set_tests_properties(polo.examples.serial.logistic.l1.nesterov PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 225.614595"
)

add_executable(logistic-l1-adagrad logistic-l1.cpp)
target_compile_definitions(logistic-l1-adagrad PRIVATE ADAGRAD)
target_link_libraries(logistic-l1-adagrad PRIVATE polo::polo)
add_test(NAME polo.examples.serial.logistic.l1.adagrad COMMAND logistic-l1-adagrad)
set_tests_properties(polo.examples.serial.logistic.l1.adagrad PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 226.557627"
)

add_executable(logistic-l1-adam logistic-l1.cpp)
target_compile_definitions(logistic-l1-adam PRIVATE ADAM)
target_link_libraries(logistic-l1-adam PRIVATE polo::polo)
add_test(NAME polo.examples.serial.logistic.l1.adam COMMAND logistic-l1-adam)
set_tests_properties(polo.examples.serial.logistic.l1.adam PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 241.091232"
)

add_executable(logistic-l1-l2-momentum logistic-l1-l2.cpp)
target_compile_definitions(logistic-l1-l2-momentum PRIVATE MOMENTUM)
target_link_libraries(logistic-l1-l2-momentum PRIVATE polo::polo)
add_test(NAME polo.examples.serial.logistic.l1.l2.momentum COMMAND logistic-l1-l2-momentum)
set_tests_properties(polo.examples.serial.logistic.l1.l2.momentum PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 226.197719"
)

add_executable(logistic-l1-l2-nesterov logistic-l1-l2.cpp)
target_compile_definitions(logistic-l1-l2-nesterov PRIVATE NESTEROV)
target_link_libraries(logistic-l1-l2-nesterov PRIVATE polo::polo)
add_test(NAME polo.examples.serial.logistic.l1.l2.nesterov COMMAND logistic-l1-l2-nesterov)
set_tests_properties(polo.examples.serial.logistic.l1.l2.nesterov PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 225.614595"
)

add_executable(logistic-l1-l2-adagrad logistic-l1-l2.cpp)
target_compile_definitions(logistic-l1-l2-adagrad PRIVATE ADAGRAD)
target_link_libraries(logistic-l1-l2-adagrad PRIVATE polo::polo)
add_test(NAME polo.examples.serial.logistic.l1.l2.adagrad COMMAND logistic-l1-l2-adagrad)
set_tests_properties(polo.examples.serial.logistic.l1.l2.adagrad PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 226.557627"
)

add_executable(logistic-l1-l2-adam logistic-l1-l2.cpp)
target_compile_definitions(logistic-l1-l2-adam PRIVATE ADAM)
target_link_libraries(logistic-l1-l2-adam PRIVATE polo::polo)
add_test(NAME polo.examples.serial.logistic.l1.l2.adam COMMAND logistic-l1-l2-adam)
set_tests_properties(polo.examples.serial.logistic.l1.l2.adam PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 241.091232"
)
