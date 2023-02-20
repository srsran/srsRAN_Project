#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
    backend_worker.cpp
    srslog.cpp
    srslog_c.cpp
    event_trace.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include/srsgnb/srslog/bundled/)
include_directories(${PROJECT_SOURCE_DIR}/include/srsgnb/srslog/formatters)

set(SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/formatters/json_formatter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/formatters/text_formatter.cpp)


add_library(srslog STATIC ${SOURCES})
target_link_libraries(srslog fmt ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS srslog EXPORT srsran_export)
