#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "ofh")

add_executable(ofh_integration_test ofh_integration_test.cpp)
target_link_libraries(ofh_integration_test
        srslog
        srsran_ru_ofh
        srsran_phy_support
        srsran_channel_precoder
        srsran_support)
add_test(ofh_integration_test ofh_integration_test -d \'{0,1}\')
set_tests_properties(ofh_integration_test PROPERTIES LABELS "tsan;NO_MEMCHECK")
