#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        src/format.cc
        src/os.cc)

add_library(fmt STATIC ${SOURCES})
target_link_libraries(fmt ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS fmt EXPORT srsran_export)
