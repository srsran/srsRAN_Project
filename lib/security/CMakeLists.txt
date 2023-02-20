#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


set(SOURCES security.cpp s3g.cpp zuc.cpp)

add_library(srsran_security STATIC ${SOURCES})
target_include_directories(srsran_security PUBLIC ${SEC_INCLUDE_DIRS})
target_link_libraries(srsran_security ${SEC_LIBRARIES})
