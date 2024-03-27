#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(build_info)
add_subdirectory(network)
add_subdirectory(version)

set(SOURCES
        backtrace.cpp
        math_utils.cpp
        executors/priority_task_queue.cpp
        executors/unique_thread.cpp
        executors/task_worker.cpp
        executors/task_worker_pool.cpp
        executors/task_execution_manager.cpp
        timers.cpp
        bit_encoding.cpp
        config_yaml.cpp
        signal_handler.cpp
        event_tracing.cpp
        sysinfo.cpp
        byte_buffer.cpp
        byte_buffer_chain.cpp
        cpu_architecture_info.cpp)

add_library(srsran_support STATIC ${SOURCES})
target_link_libraries(srsran_support srsran_network ${CMAKE_THREAD_LIBS_INIT} ${YAMLCPP_LIBRARY}
                      ${NUMA_LIBRARIES} srslog)
add_backward(srsran_support)

if (Backward_FOUND AND BACKWARD_HAS_EXTERNAL_LIBRARIES)
    set_source_files_properties(backtrace.cpp APPEND PROPERTIES COMPILE_DEFINITIONS "HAVE_BACKWARD")
    set_source_files_properties(backtrace.cpp APPEND PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/external)
endif (Backward_FOUND AND BACKWARD_HAS_EXTERNAL_LIBRARIES)

install(TARGETS srsran_support
        EXPORT srsran_export)
