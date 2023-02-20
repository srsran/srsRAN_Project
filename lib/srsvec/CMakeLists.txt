#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        add.cpp
        aligned_vec.cpp
        bit.cpp
        clip.cpp
        compare.cpp
        conversion.cpp
        dot_prod.cpp
        prod.cpp
        sc_prod.cpp
        subtract.cpp
        )

include_directories(${PROJECT_SOURCE_DIR}/include/srsgnb/srsvec)

add_library(srsvec STATIC ${SOURCES})
target_link_libraries(srsvec srslog)