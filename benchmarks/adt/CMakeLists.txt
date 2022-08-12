#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(bounded_bitset_benchmark bounded_bitset_count_benchmark.cpp)
target_link_libraries(bounded_bitset_benchmark srslog)
add_test(bounded_bitset_benchmark bounded_bitset_benchmark -R 100)

add_executable(bounded_bitset_any_benchmark bounded_bitset_any_benchmark.cpp)
target_link_libraries(bounded_bitset_any_benchmark srslog)
add_test(bounded_bitset_any_benchmark bounded_bitset_any_benchmark -R 100)

