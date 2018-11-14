cmake_minimum_required(VERSION 3.9.0)

project(polo-api)

if (NOT TARGET polo::polo)
  find_package(polo CONFIG REQUIRED)
endif()

add_library(poloapi
  include/polo_api.h
  include/polo_aux.hpp
  src/polo_adadelta_s.cpp
  src/polo_adadelta_cr.cpp
  src/polo_adadelta_psm.cpp
  src/polo_adadelta_pss.cpp
  src/polo_adadelta_psw.cpp
  src/polo_adagrad_s.cpp
  src/polo_adagrad_cr.cpp
  src/polo_adagrad_psm.cpp
  src/polo_adagrad_pss.cpp
  src/polo_adagrad_psw.cpp
  src/polo_adam_s.cpp
  src/polo_adam_cr.cpp
  src/polo_adam_psm.cpp
  src/polo_adam_pss.cpp
  src/polo_adam_psw.cpp
  src/polo_gradient_s.cpp
  src/polo_gradient_cr.cpp
  src/polo_gradient_psm.cpp
  src/polo_gradient_pss.cpp
  src/polo_gradient_psw.cpp
  src/polo_momentum_s.cpp
  src/polo_momentum_cr.cpp
  src/polo_momentum_psm.cpp
  src/polo_momentum_pss.cpp
  src/polo_momentum_psw.cpp
  src/polo_nadam_s.cpp
  src/polo_nadam_cr.cpp
  src/polo_nadam_psm.cpp
  src/polo_nadam_pss.cpp
  src/polo_nadam_psw.cpp
  src/polo_nesterov_s.cpp
  src/polo_nesterov_cr.cpp
  src/polo_nesterov_psm.cpp
  src/polo_nesterov_pss.cpp
  src/polo_nesterov_psw.cpp
  src/polo_proxgrad_s.cpp
  src/polo_proxgrad_cr.cpp
  src/polo_proxgrad_psm.cpp
  src/polo_proxgrad_pss.cpp
  src/polo_proxgrad_psw.cpp
)
target_include_directories(poloapi PUBLIC include)
target_link_libraries(poloapi PRIVATE polo::polo)
set_target_properties(poloapi PROPERTIES PUBLIC_HEADER include/polo_api.h)

install(TARGETS poloapi
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  PUBLIC_HEADER DESTINATION include
)
