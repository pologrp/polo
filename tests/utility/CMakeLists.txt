add_executable(sampler_uniform sampler_uniform.cpp)
target_link_libraries(sampler_uniform
  PRIVATE
    polo::polo
    GTest::Main
)
add_test(NAME polo.utility.sampler.uniform COMMAND sampler_uniform)

add_executable(sampler_custom sampler_custom.cpp)
target_link_libraries(sampler_custom
  PRIVATE
    polo::polo
    GTest::Main
)
add_test(NAME polo.utility.sampler.custom COMMAND sampler_custom)
