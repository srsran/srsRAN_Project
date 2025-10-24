#
# Copyright 2021-2025 Software Radio Systems Limited
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

set_directory_properties(PROPERTIES LABELS "pretty_printers")

add_executable(pretty_printer_bf16_test pretty_printer_bf16_test.cpp)
add_test(NAME pretty_printer_bf16_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_bf16_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = 1.2265625'"
)

add_executable(pretty_printer_bounded_bitset_test pretty_printer_bounded_bitset_test.cpp)
target_link_libraries(pretty_printer_bounded_bitset_test PRIVATE fmt)
add_test(NAME pretty_printer_bounded_bitset_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_bounded_bitset_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = bounded_bitset of length 4, capacity 5 = 0100'"
)

add_executable(pretty_printer_cbf16_test pretty_printer_cbf16_test.cpp)
add_test(NAME pretty_printer_cbf16_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_cbf16_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = 1.2265625 + 4.5625i'"
)

add_executable(pretty_printer_slotted_array_test pretty_printer_slotted_array_test.cpp)
target_link_libraries(pretty_printer_slotted_array_test PRIVATE fmt)
add_test(NAME pretty_printer_slotted_array_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_slotted_array_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = slotted_array of 2 elements, capacity 5 = {\\[1\\] = 3, \\[3\\] = -5}'"
)

add_executable(pretty_printer_slotted_vector_test pretty_printer_slotted_vector_test.cpp)
add_test(NAME pretty_printer_slotted_vector_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_slotted_vector_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = slotted_vector of 2 elements = {\\[1\\] = 3, \\[3\\] = -5}'"
)

add_executable(pretty_printer_static_vector_test pretty_printer_static_vector_test.cpp)
target_link_libraries(pretty_printer_static_vector_test PRIVATE fmt)
add_test(NAME pretty_printer_static_vector_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_static_vector_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = static_vector of length 3, capacity 5 = {0, 1, 2}'"
)

add_executable(pretty_printer_tiny_optional_test pretty_printer_tiny_optional_test.cpp)
add_test(NAME pretty_printer_tiny_optional_empty_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_tiny_optional_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'c = tiny_optional (empty)'"
)
add_test(NAME pretty_printer_tiny_optional_alternative_empty_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_tiny_optional_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'd = tiny_optional (empty)'"
)
add_test(NAME pretty_printer_tiny_optional_present_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_tiny_optional_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = tiny_optional (present) = {std::unique_ptr<int> = {get() = 0x[0-9a-f]\\{12\\}}}'"
)
add_test(NAME pretty_printer_tiny_optional_alternative_present_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_tiny_optional_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'b = tiny_optional (present) = {3.4000001}'"
)
