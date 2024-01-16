#
# Copyright 2021-2024 Software Radio Systems Limited
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
        backend_worker.cpp
        srslog.cpp
        srslog_c.cpp
        event_trace.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include/srsran/srslog/bundled/)
include_directories(${PROJECT_SOURCE_DIR}/include/srsran/srslog/formatters)

set(SOURCES
        ${SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/formatters/json_formatter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/formatters/text_formatter.cpp)


add_library(srslog STATIC ${SOURCES})
target_link_libraries(srslog fmt ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS srslog EXPORT srsran_export)
