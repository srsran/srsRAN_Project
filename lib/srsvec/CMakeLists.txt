#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        add.cpp
        aligned_vec.cpp
        bit.cpp
        compare.cpp
        conversion.cpp
        dot_prod.cpp
        prod.cpp
        sc_prod.cpp
        clip.cpp
        )

include_directories(${PROJECT_SOURCE_DIR}/include/srsgnb/srsvec)

add_library(srsvec STATIC ${SOURCES})
INSTALL(TARGETS srsvec DESTINATION ${LIBRARY_DIR})

install(TARGETS srsvec EXPORT srsgnb_export)
