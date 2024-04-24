#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
            du_low_config_cli11_schema.cpp
            du_low_config_validator.cpp)

add_library(srsran_du_low_unit_helpers STATIC ${SOURCES})
target_include_directories(srsran_du_low_unit_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_du_low_unit_helpers srsran_upper_phy)
