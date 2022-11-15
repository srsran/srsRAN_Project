#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(io_broker)

set(SOURCES
        math_utils.cpp
        task_worker.cpp
        unique_thread.cpp
        timers.cpp
        bit_encoding.cpp)

add_library(srsgnb_support STATIC ${SOURCES})
target_link_libraries(srsgnb_support srsgnb_io_broker ${CMAKE_THREAD_LIBS_INIT} srslog)

install(TARGETS srsgnb_support EXPORT srsgnb_export)
