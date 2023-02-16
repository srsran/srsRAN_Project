#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "security")

add_executable(security_test security_test.cpp)
target_link_libraries(security_test srsgnb_security srsgnb_support srslog gtest gtest_main)
gtest_discover_tests(security_test)
