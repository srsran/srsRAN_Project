#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "ran")

add_executable(dci_packing_test pdcch/dci_packing_test.cpp)
target_link_libraries(dci_packing_test srsgnb_support srsgnb_ran)
add_test(dci_packing_test dci_packing_test)

add_executable(pdcch_candidates pdcch/pdcch_candidates_common_test.cpp)
target_link_libraries(pdcch_candidates srsgnb_support srsgnb_ran)
add_test(pdcch_candidates pdcch_candidates)

add_executable(resource_allocation_frequency_test resource_allocation/resource_allocation_frequency_test.cpp)
target_link_libraries(resource_allocation_frequency_test srsgnb_support srsgnb_ran)
add_test(resource_allocation_frequency_test resource_allocation_frequency_test)

add_executable(slot_point_test slot_point_test.cpp)
target_link_libraries(slot_point_test srsgnb_support)
add_test(slot_point_test slot_point_test)

add_executable(band_helper_test band_helper_test.cpp)
target_link_libraries(band_helper_test srsgnb_ran srslog)
target_include_directories(band_helper_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(band_helper_test band_helper_test)
