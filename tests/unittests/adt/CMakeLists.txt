#
# Copyright 2021-2024 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

set_directory_properties(PROPERTIES LABELS "adt")

add_executable(bounded_bitset_test bounded_bitset_test.cpp)
target_link_libraries(bounded_bitset_test srslog gtest gtest_main)
add_test(bounded_bitset_test bounded_bitset_test)

add_executable(interval_test interval_test.cpp)
target_link_libraries(interval_test srslog gtest gtest_main)
add_test(interval_test interval_test)

add_executable(byte_buffer_test
        byte_buffer_segment_test.cpp
        byte_buffer_test.cpp
        byte_buffer_chain_test.cpp)
target_link_libraries(byte_buffer_test srslog srsran_support gtest gtest_main)
add_test(byte_buffer_test byte_buffer_test)
set_tests_properties(byte_buffer_test PROPERTIES LABELS "tsan")
gtest_discover_tests(byte_buffer_test)

add_executable(ring_buffer_test ring_buffer_test.cpp blocking_queue_test.cpp concurrent_queue_test.cpp)
target_link_libraries(ring_buffer_test srslog srsran_support gtest gtest_main)
add_test(ring_buffer_test ring_buffer_test)
set_tests_properties(ring_buffer_test PROPERTIES LABELS "tsan")
gtest_discover_tests(ring_buffer_test)

add_executable(circular_map_test circular_map_test.cpp)
target_link_libraries(circular_map_test srslog)
add_test(circular_map_test circular_map_test)

add_executable(circular_map_test_alt circular_map_test_alt.cpp)
target_link_libraries(circular_map_test_alt srslog gtest gtest_main)
gtest_discover_tests(circular_map_test_alt)

add_executable(expected_test expected_test.cpp)
target_link_libraries(expected_test srslog)
add_test(expected_test expected_test)

add_executable(optional_test optional_test.cpp)
target_link_libraries(optional_test srslog gtest gtest_main)
add_test(optional_test optional_test)

add_executable(tiny_optional_test tiny_optional_test.cpp)
target_link_libraries(tiny_optional_test srslog gtest gtest_main)
add_test(tiny_optional_test tiny_optional_test)

add_executable(slotted_array_test slotted_array_test.cpp)
target_link_libraries(slotted_array_test srslog gtest gtest_main)
add_test(slotted_array_test slotted_array_test)

add_executable(span_formatter_test span_formatter_test.cpp)
target_link_libraries(span_formatter_test srslog)
add_test(span_formatter_test span_formatter_test)

add_executable(static_vector_test static_vector_test.cpp)
target_link_libraries(static_vector_test srslog gtest gtest_main)
add_test(static_vector_test static_vector_test)

add_executable(unique_function_test unique_function_test.cpp)
target_link_libraries(unique_function_test srslog)
add_test(unique_function_test unique_function_test)

add_executable(filter_view_test filter_view_test.cpp)
target_link_libraries(filter_view_test srslog)
add_test(filter_view_test filter_view_test)

add_executable(strong_type_test strong_type_test.cpp)
target_link_libraries(strong_type_test srslog gtest gtest_main)
add_test(strong_type_test strong_type_test)

add_executable(bounded_integer_test bounded_integer_test.cpp)
target_link_libraries(bounded_integer_test srslog gtest gtest_main)
gtest_discover_tests(bounded_integer_test)
