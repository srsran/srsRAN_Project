#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(dft_processor_test dft_processor_test.cpp)
target_link_libraries(dft_processor_test srslog srsgnb_generic_funcs)

add_test(dft_processor_generic_test dft_processor_test -F generic -R 10)
add_test(dft_processor_fftx_test dft_processor_test -F fftx -R 10)
if (ENABLE_FFTW AND FFTW3F_FOUND)
    add_test(dft_processor_fftw_test dft_processor_test -F fftw -R 10)
endif (ENABLE_FFTW AND FFTW3F_FOUND)

add_executable(dft_processor_vectortest dft_processor_vectortest.cpp)
target_link_libraries(dft_processor_vectortest srslog srsgnb_generic_funcs gtest gtest_main)

add_test(dft_processor_vectortest dft_processor_vectortest)

add_custom_command(
  TARGET dft_processor_vectortest
  POST_BUILD
  WORKING_DIRECTORY ${TEST_DATA_DIR}
  COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_CURRENT_SOURCE_DIR}/dft_processor_test_data.tar.gz
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dft_processor_test_data.tar.gz
  VERBATIM)
