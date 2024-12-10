#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        nrppa_dummy_factory.cpp
        nrppa_dummy_impl.cpp
    )


add_library(srsran_nrppa STATIC ${SOURCES})

target_link_libraries(srsran_nrppa
	srsran_cu_cp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
