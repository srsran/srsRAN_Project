file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(short_block_encoder_test short_block_encoder_test.cpp)
target_link_libraries(short_block_encoder_test short_block srslog)
add_custom_command(
    TARGET short_block_encoder_test
    POST_BUILD
    WORKING_DIRECTORY ${TEST_DATA_DIR}
    COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/short_block_encoder_test_data.tar.gz
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/short_block_encoder_test_data.tar.gz
    VERBATIM
    )
add_test(short_block_encoder_test short_block_encoder_test)

