#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(versioning)
add_subdirectory(network)

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
	dynlink_manager.cpp
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
