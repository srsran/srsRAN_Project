#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES du_cell_config_validation.cpp du_update_config_helpers.cpp)
add_library(srsran_du_config_validators STATIC ${SOURCES})

add_library(srsran_du STATIC
        du_wrapper_impl.cpp
        du_wrapper_factory.cpp
        du_high_wrapper_factory.cpp
        du_high_wrapper_impl.cpp
        adapters/fapi_factory.cpp)
target_link_libraries(srsran_du PUBLIC srsran_du_config_validators srsran_du_high srsran_du_low)
