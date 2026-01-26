#
# Copyright 2021-2026 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

set(SOURCES
        traces/critical_traces.cpp
        traces/du_traces.cpp
        traces/ru_traces.cpp
        traces/up_traces.cpp
        traces/ofh_traces.cpp)

set(LIBS srsran_support)

if(ENABLE_SRSRAN_TRACY)
	list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/external/tracy/public/TracyClient.cpp)
	list(APPEND LIBS ${TRACY_LIBRARIES})
endif()

add_library(srsran_instrumentation ${SOURCES})

if(ENABLE_SRSRAN_TRACY)
  target_include_directories(srsran_instrumentation PUBLIC ${CMAKE_SOURCE_DIR}/external/tracy/public/)
endif()

target_link_libraries(srsran_instrumentation ${LIBS})

add_to_exported_libs(srsran_instrumentation)
