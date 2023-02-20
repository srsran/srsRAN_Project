#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES sdap_factory.cpp)

add_library(srsran_sdap STATIC ${SOURCES})
target_link_libraries(srsran_sdap)
