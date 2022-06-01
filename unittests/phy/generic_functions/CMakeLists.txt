#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(dft_processor_fftw_test dft_processor_fftw_test.cpp)
target_link_libraries(dft_processor_fftw_test srslog srsgnb_generic_funcs)
add_test(dft_processor_fftw_test dft_processor_fftw_test)
