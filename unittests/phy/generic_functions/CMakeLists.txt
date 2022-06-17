#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(dft_processor_test dft_processor_test.cpp)
target_link_libraries(dft_processor_test srslog srsgnb_generic_funcs)

add_test(dft_processor_generic_test dft_processor_test -F generic -R 10)
add_test(dft_processor_fftx_test dft_processor_test -F fftx -R 10)
if (ENABLE_FFTW AND FFTW3F_FOUND)
    add_test(dft_processor_fftw_test dft_processor_test -F fftw -R 10)
endif (ENABLE_FFTW AND FFTW3F_FOUND)

add_executable(dft_processor_benchmark dft_processor_benchmark.cpp)
target_link_libraries(dft_processor_benchmark srslog srsgnb_generic_funcs)

add_test(dft_processor_generic_benchmark dft_processor_benchmark -F generic -R 10 -s)
add_test(dft_processor_fftx_benchmark dft_processor_benchmark -F fftx -R 10 -s)
if (ENABLE_FFTW AND FFTW3F_FOUND)
    add_test(dft_processor_fftw_benchmark dft_processor_benchmark -F fftw -R 10 -s)
endif (ENABLE_FFTW AND FFTW3F_FOUND)