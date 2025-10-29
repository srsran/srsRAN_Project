#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
  rohc_factory.cpp
  rohc_support.cpp
)

add_library(srsran_rohc STATIC ${SOURCES})

set(SRSGNB_ROHC_DEFINITIONS "")
set(SRSGNB_ROHC_LIBRARIES "")

if(ROHC_FOUND)
  add_subdirectory(rohc_lib)
  list(APPEND SRSGNB_ROHC_DEFINITIONS -DENABLE_ROHC_LIB)
  list(APPEND SRSGNB_ROHC_LIBRARIES srsran_rohc_lib)
endif()

add_definitions(${SRSGNB_ROHC_DEFINITIONS})

target_link_libraries(srsran_rohc ${SRSGNB_ROHC_LIBRARIES})
