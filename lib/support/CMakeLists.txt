#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        dft_processor_fftw_impl.cpp
        math_utils.cpp
        task_worker.cpp
        threads.cpp
        timers.cpp)

add_library(srsgnb_support STATIC ${SOURCES})
target_link_libraries(srsgnb_support srslog ${FFTW3F_LIBRARIES} srsvec)
