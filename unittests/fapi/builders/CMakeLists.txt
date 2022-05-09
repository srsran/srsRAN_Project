#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "fapi")

add_executable(dl_pdcch_builder_test pdcch_test.cpp)
target_link_libraries(dl_pdcch_builder_test srsgnb_support srslog)
add_test(dl_pdcch_builder_test dl_pdcch_builder_test)

add_executable(dl_pdsch_builder_test dl_pdsch_builder_test.cpp)
target_link_libraries(dl_pdsch_builder_test srsgnb_support srslog)
add_test(dl_pdsch_builder_test dl_pdsch_builder_test)
