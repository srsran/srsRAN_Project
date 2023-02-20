#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "fapi_adaptor")

add_subdirectory(mac)
add_subdirectory(phy)

add_executable(fapi_adaptor_performance_dl_tti_request adaptor_performance_dl_tti_request.cpp)
target_link_libraries(fapi_adaptor_performance_dl_tti_request srsran_fapi_phy_message_adaptors srsran_mac_fapi_adaptors srslog)
add_test(fapi_adaptor_performance_dl_tti_request fapi_adaptor_performance_dl_tti_request)

