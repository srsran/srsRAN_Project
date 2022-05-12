#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "fapi_adaptor")

add_executable(fapi_adaptor_test fapi_adaptor_test.cpp)
target_link_libraries(fapi_adaptor_test ssb_fapi_adaptor srslog)
add_test(fapi_adaptor_test fapi_adaptor_test)

add_executable(fapi_adaptor_performance_dl_tti_request adaptor_performance_dl_tti_request.cpp)
target_link_libraries(fapi_adaptor_performance_dl_tti_request ssb_fapi_adaptor srslog pdcch_fapi_adaptor)
add_test(fapi_adaptor_performance_dl_tti_request fapi_adaptor_performance_dl_tti_request)

add_executable(pdcch_adaptor_test pdcch_adaptor_test.cpp)
target_link_libraries(pdcch_adaptor_test pdcch_fapi_adaptor srslog)
add_test(pdcch_adaptor_test pdcch_adaptor_test)

add_executable(pdsch_adaptor_test pdsch_adaptor_test.cpp)
target_link_libraries(pdsch_adaptor_test pdsch_fapi_adaptor srslog)
add_test(pdsch_adaptor_test pdsch_adaptor_test)
