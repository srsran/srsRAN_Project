#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


set(SOURCES
  ciphering_engine_nea1.cpp
  ciphering_engine_nea2.cpp
  ciphering_engine_nea3.cpp
  integrity_engine_generic.cpp
  security.cpp
  security_engine_impl.cpp
  s3g.cpp
  zuc.cpp
)

add_library(srsran_security STATIC ${SOURCES})
target_include_directories(srsran_security PUBLIC ${SEC_INCLUDE_DIRS})
target_link_libraries(srsran_security ${SEC_LIBRARIES})
