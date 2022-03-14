
add_executable(ldpc_enc_dec_test ldpc_enc_dec_test.cpp)
target_link_libraries(ldpc_enc_dec_test ldpc srslog)
add_custom_command(TARGET ldpc_enc_dec_test
    POST_BUILD
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/examplesBG?.dat ./
    )
add_test(ldpc_enc_dec_test ldpc_enc_dec_test)

add_executable(ldpc_rm_test ldpc_rm_test.cpp)
target_link_libraries(ldpc_rm_test ldpc srslog)
add_test(ldpc_rm_test ldpc_rm_test)
