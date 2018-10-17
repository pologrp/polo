cmake_minimum_required(VERSION 3.9.0)

project(polo-api VERSION 1.0.0 LANGUAGES C CXX)

add_library(poloapi
  src/api.cpp
  src/custom-api.cpp
)
target_include_directories(poloapi PUBLIC include)
target_link_libraries(poloapi PRIVATE polo::polo)

add_library(psapi
  src/paramserver_m.cpp
  src/paramserver_s.cpp
  src/paramserver_w.cpp
)
target_include_directories(psapi PUBLIC include)
target_link_libraries(psapi PRIVATE polo::polo)

install(TARGETS poloapi psapi
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
