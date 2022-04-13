
set_directory_properties(PROPERTIES LABELS "phy")

add_executable(crc_calculator_test crc_calculator_test.cpp)
target_link_libraries(crc_calculator_test crc_calculator srsvec srslog)
add_test(crc_calculator_test crc_calculator_test)

add_subdirectory(polar)
add_subdirectory(ldpc)
