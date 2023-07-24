#
# Copyright 2021-2023 Software Radio Systems Limited
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

add_subdirectory(build_info)
add_subdirectory(network)
add_subdirectory(version)

set(SOURCES
        backtrace.cpp
        math_utils.cpp
        unique_thread.cpp
        task_worker.cpp
        task_worker_pool.cpp
        timers.cpp
        bit_encoding.cpp
        config_json.cpp
        config_yaml.cpp
        signal_handler.cpp
        event_tracing.cpp)

add_library(srsran_support STATIC ${SOURCES})
target_link_libraries(srsran_support srsran_network ${CMAKE_THREAD_LIBS_INIT} ${YAMLCPP_LIBRARY} srslog)
add_backward(srsran_support)

if (Backward_FOUND AND BACKWARD_HAS_EXTERNAL_LIBRARIES)
    set_source_files_properties(backtrace.cpp APPEND PROPERTIES COMPILE_DEFINITIONS "HAVE_BACKWARD")
    set_source_files_properties(backtrace.cpp APPEND PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/external)
endif (Backward_FOUND AND BACKWARD_HAS_EXTERNAL_LIBRARIES)

install(TARGETS srsran_support
        EXPORT srsran_export)