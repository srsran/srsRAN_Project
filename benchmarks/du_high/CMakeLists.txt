#
#Copyright 2013 - 2022 Software Radio Systems Limited
#
#By using this file, you agree to the terms and conditions set
#forth in the LICENSE file which can be found at the top level of
#the distribution.
#

set_directory_properties(PROPERTIES LABELS "du_high")

include_directories(../..)

add_executable(du_high_benchmark du_high_benchmark.cpp)
target_link_libraries(du_high_benchmark srsgnb_du_high f1_du_test_helpers gtest)
