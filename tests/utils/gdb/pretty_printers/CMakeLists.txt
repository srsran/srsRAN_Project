#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "pretty_printers")

add_executable(pretty_printer_bf16_test pretty_printer_bf16_test.cpp)
add_test(NAME pretty_printer_bf16_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_bf16_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = 1.2265625'"
)

add_executable(pretty_printer_cbf16_test pretty_printer_cbf16_test.cpp)
add_test(NAME pretty_printer_cbf16_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_cbf16_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = 1.2265625 + 4.5625i'"
)

add_executable(pretty_printer_static_vector_test pretty_printer_static_vector_test.cpp)
target_link_libraries(pretty_printer_static_vector_test PRIVATE fmt)
add_test(NAME pretty_printer_static_vector_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_static_vector_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = static_vector of length 3, capacity 5 = {0, 1, 2}'"
)

add_executable(pretty_printer_tiny_optional_test pretty_printer_tiny_optional_test.cpp)
add_test(NAME pretty_printer_tiny_optional_empty_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_tiny_optional_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'c = \"tiny_optional (empty)\"'"
)
add_test(NAME pretty_printer_tiny_optional_alternative_empty_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_tiny_optional_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'd = \"tiny_optional (empty)\"'"
)
add_test(NAME pretty_printer_tiny_optional_present_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_tiny_optional_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = \"tiny_optional (present)\" = {\\[0\\] = std::unique_ptr<int> = {get() = 0x[0-9a-f]\\{12\\}}}'"
)
add_test(NAME pretty_printer_tiny_optional_alternative_present_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_tiny_optional_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'b = \"tiny_optional (present)\" = {\\[0\\] = 3.4000001}'"
)
