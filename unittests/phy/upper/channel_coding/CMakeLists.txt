
add_executable(crc_calculator_test crc_calculator_test.cpp)
target_link_libraries(crc_calculator_test crc_calculator srsvec srslog)
add_test(crc_calculator_test crc_calculator_test)

add_executable(polar_chain_test polar_chain_test.cpp)
target_link_libraries(polar_chain_test polar srsvec srslog)
add_test(polar_chain_test_bch polar_chain_test -n9 -k56 -e864 -i0 -s101 -o1)
add_test(polar_chain_test_dci polar_chain_test -n9 -k40 -e100 -i0 -s101 -o1)
add_test(polar_chain_test_uci polar_chain_test -n10 -k20 -e256 -i1 -s101 -o1)
add_test(polar_chain_test_uci2 polar_chain_test -n10 -k18 -e45 -i1 -s101 -o1)
add_test(polar_chain_test_uci3 polar_chain_test -n10 -k18 -e38 -i1 -s101 -o1)

add_executable(polar_interleaver_test polar_interleaver_test.cpp)
target_link_libraries(polar_interleaver_test polar srsvec srslog)
add_test(polar_interleaver_test polar_interleaver_test)

add_executable(ldpc_enc_dec_test ldpc_enc_dec_test.cpp)
target_link_libraries(ldpc_enc_dec_test ldpc srslog)
add_custom_command(TARGET ldpc_enc_dec_test
    POST_BUILD
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/examplesBG?.dat ./
    )
add_test(ldpc_enc_dec_test ldpc_enc_dec_test)

add_executable(ldpc_rm_test ldpc_rm_test.cpp)
target_link_libraries(ldpc_rm_test ldpc srslog)
