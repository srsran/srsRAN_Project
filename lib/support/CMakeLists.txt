#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        math_utils.cpp
        task_worker.cpp
        unique_thread.cpp
        timers.cpp)

add_library(srsgnb_support STATIC ${SOURCES})
target_link_libraries(srsgnb_support srslog pthread)
