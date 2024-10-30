#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        accumulate.cpp
        add.cpp
        bit.cpp
        clip.cpp
        compare.cpp
        conversion.cpp
        convolution.cpp
        division.cpp
        dot_prod.cpp
        modulus_square.cpp
        prod.cpp
        sc_prod.cpp
        subtract.cpp
        unwrap.cpp
)

add_library(srsvec STATIC ${SOURCES})
target_link_libraries(srsvec srslog)

add_to_exported_libs(srsvec)
