#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

remove_definitions(-DASSERTS_ENABLED)
remove_definitions(-DPARANOID_ASSERTS_ENABLED)

add_executable(bounded_bitset_benchmark bounded_bitset_benchmark.cpp)
target_link_libraries(bounded_bitset_benchmark srslog)
add_test(bounded_bitset_benchmark bounded_bitset_benchmark -R 100)