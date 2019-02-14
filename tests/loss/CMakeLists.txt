add_executable(leastsquares leastsquares.cpp)
target_link_libraries(leastsquares
  PRIVATE
    polo::polo
    GTest::Main
)
add_test(NAME polo.loss.leastsquares COMMAND leastsquares)

add_executable(logistic logistic.cpp)
target_link_libraries(logistic
  PRIVATE
    polo::polo
    GTest::Main
)
add_test(NAME polo.loss.logistic COMMAND logistic)
