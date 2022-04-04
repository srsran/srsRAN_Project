
set_directory_properties(PROPERTIES LABELS "phy")

add_executable(ldpc_enc_dec_test ldpc_enc_dec_test.cpp)
target_link_libraries(ldpc_enc_dec_test ldpc srslog)
add_custom_command(
    TARGET ldpc_enc_dec_test
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/unittests/phy/upper/channel_coding/ldpc
    COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/ldpc_encoder_test_data.tar.gz
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ldpc_encoder_test_data.tar.gz
    VERBATIM
    )
add_test(ldpc_enc_dec_test ldpc_enc_dec_test)

add_executable(ldpc_rm_test ldpc_rm_test.cpp)
target_link_libraries(ldpc_rm_test ldpc srslog)
add_test(ldpc_rm_test ldpc_rm_test)

add_executable(ldpc_segmenter_test ldpc_segmenter_test.cpp)
target_link_libraries(ldpc_segmenter_test ldpc srslog)
add_custom_command(
    TARGET ldpc_segmenter_test
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/unittests/phy/upper/channel_coding/ldpc
    COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/ldpc_segmenter_test_data.tar.gz
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ldpc_segmenter_test_data.tar.gz
    VERBATIM
    )
add_test(ldpc_segmenter_test ldpc_segmenter_test)
