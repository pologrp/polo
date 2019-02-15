add_executable(gradient cpp/gradient.cpp)
target_link_libraries(gradient
  PRIVATE
    polo::polo
)
