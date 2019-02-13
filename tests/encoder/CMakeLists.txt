add_executable(dynamic dynamic.cpp)
target_link_libraries(dynamic
  PRIVATE
    polo::polo
    GTest::Main
)
add_test(NAME polo.encoder.dynamic COMMAND dynamic)

add_executable(identity identity.cpp)
target_link_libraries(identity
  PRIVATE
    polo::polo
    GTest::Main
)
add_test(NAME polo.encoder.identity COMMAND identity)

add_executable(random_quantizer random_quantizer.cpp)
target_link_libraries(random_quantizer
  PRIVATE
    polo::polo
    GTest::Main
)
add_test(NAME polo.encoder.random_quantizer COMMAND random_quantizer)

add_executable(random_sparsifier random_sparsifier.cpp)
target_link_libraries(random_sparsifier
  PRIVATE
    polo::polo
    GTest::Main
)
add_test(NAME polo.encoder.random_sparsifier COMMAND random_sparsifier)

add_executable(ternary ternary.cpp)
target_link_libraries(ternary
  PRIVATE
    polo::polo
    GTest::Main
)
add_test(NAME polo.encoder.ternary COMMAND ternary)

add_executable(topk topk.cpp)
target_link_libraries(topk
  PRIVATE
    polo::polo
    GTest::Main
)
add_test(NAME polo.encoder.topk COMMAND topk)
