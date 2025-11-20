#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        traces/critical_traces.cpp
        traces/du_traces.cpp
        traces/ru_traces.cpp
        traces/up_traces.cpp
        traces/ofh_traces.cpp)

if(ENABLE_SRSRAN_TRACY)
	list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/external/tracy/public/TracyClient.cpp)
endif()

add_library(srsran_instrumentation ${SOURCES})

target_include_directories(srsran_instrumentation PUBLIC ${CMAKE_SOURCE_DIR}/external/tracy/public/)

target_link_libraries(srsran_instrumentation srsran_support ${TRACY_LIBRARIES})

add_to_exported_libs(srsran_instrumentation)
