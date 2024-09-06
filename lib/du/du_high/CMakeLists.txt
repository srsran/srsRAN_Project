#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(adapters)
add_subdirectory(du_manager)

add_library(srsran_du_high STATIC
        du_high_impl.cpp
        du_high_factory.cpp)
target_link_libraries(srsran_du_high PUBLIC srsran_du_high_adapters)

add_library(srsran_du_high_wrapper STATIC
        du_high_wrapper_factory.cpp
        du_high_wrapper_impl.cpp)
target_link_libraries(srsran_du_high_wrapper PUBLIC srsran_du_config_validators srsran_du_high srsran_mac_fapi_adaptor srsran_fapi)
