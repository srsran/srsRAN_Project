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

add_subdirectory(build_info)
add_subdirectory(network)
add_subdirectory(version)

set(SOURCES
        executors/priority_task_queue.cpp
        executors/priority_task_worker.cpp
        executors/task_execution_manager.cpp
        executors/task_worker.cpp
        executors/task_worker_pool.cpp
        executors/unique_thread.cpp
        backtrace.cpp
        bit_encoding.cpp
        byte_buffer.cpp
        byte_buffer_chain.cpp
        config_yaml.cpp
        cpu_architecture_info.cpp
        event_tracing.cpp
        signal_handling.cpp
        sysinfo.cpp
        timers.cpp
        math_utils.cpp
        )

add_library(srsran_support STATIC ${SOURCES})
target_link_libraries(srsran_support srsran_network ${CMAKE_THREAD_LIBS_INIT} ${YAMLCPP_LIBRARY}
                      ${NUMA_LIBRARIES} srslog)
add_backward(srsran_support)

if (Backward_FOUND AND BACKWARD_HAS_EXTERNAL_LIBRARIES)
    set_source_files_properties(backtrace.cpp APPEND PROPERTIES COMPILE_DEFINITIONS "HAVE_BACKWARD")
    set_source_files_properties(backtrace.cpp APPEND PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/external)
endif (Backward_FOUND AND BACKWARD_HAS_EXTERNAL_LIBRARIES)

add_to_exported_libs(srsran_support)
