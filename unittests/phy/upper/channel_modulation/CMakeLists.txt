file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(modulation_mapper_test modulation_mapper_test.cpp)
target_link_libraries(modulation_mapper_test modulation_mapper srsvec srslog)
add_test(modulation_mapper_test modulation_mapper_test)

add_custom_command(
        TARGET modulation_mapper_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/modulation_mapper_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/modulation_mapper_test_data.tar.gz
        VERBATIM)

add_executable(demodulation_mapper_test demodulation_mapper_test.cpp)
target_link_libraries(demodulation_mapper_test modulation_mapper srsvec srslog)

add_custom_command(
        TARGET demodulation_mapper_test
        POST_BUILD
        WORKING_DIRECTORY ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/demodulation_mapper_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/demodulation_mapper_test_data.tar.gz
        VERBATIM)

add_test(demodulation_mapper_test demodulation_mapper_test)
