#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(versioning)

set(SOURCES
        executors/priority_task_queue.cpp
        executors/priority_task_worker.cpp
        executors/task_execution_manager.cpp
        executors/task_worker.cpp
        executors/task_worker_pool.cpp
        executors/unique_thread.cpp
        executors/execution_context_description.cpp
        executors/executor_decoration_factory.cpp
        network/io_broker_epoll.cpp
        network/io_broker_factory.cpp
        network/io_timer_source.cpp
        network/sctp_socket.cpp
        network/sockets.cpp
        network/transport_layer_address.cpp
        tracing/resource_usage.cpp
        tracing/event_tracing.cpp
        backtrace.cpp
        bit_encoding.cpp
        byte_buffer.cpp
        byte_buffer_chain.cpp
        config_yaml.cpp
        cpu_architecture_info.cpp
        resource_usage/powercap_energy_reader_impl.cpp
        resource_usage/rapl_msr_energy_reader_impl.cpp
        resource_usage/power_consumption.cpp
        resource_usage/resource_usage_utils.cpp
        rate_limiting/token_bucket.cpp
        signal_handling.cpp
        sysinfo.cpp
        timers.cpp
        math_utils.cpp
)

add_library(srsran_support STATIC ${SOURCES})
target_link_libraries(srsran_support PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${YAMLCPP_LIBRARY}
        ${NUMA_LIBRARIES} srslog)
add_backward(srsran_support)

if (Backward_FOUND AND BACKWARD_HAS_EXTERNAL_LIBRARIES)
    set_source_files_properties(backtrace.cpp APPEND PROPERTIES COMPILE_DEFINITIONS "HAVE_BACKWARD")
    set_source_files_properties(backtrace.cpp APPEND PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/external)
endif (Backward_FOUND AND BACKWARD_HAS_EXTERNAL_LIBRARIES)

add_to_exported_libs(srsran_support)
