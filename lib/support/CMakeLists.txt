#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(build_info)
add_subdirectory(io_broker)

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
        signal_handler.cpp)

add_library(srsran_support STATIC ${SOURCES})
target_link_libraries(srsran_support srsran_io_broker ${CMAKE_THREAD_LIBS_INIT} ${YAMLCPP_LIBRARY} srslog)
add_backward(srsran_support)

install(TARGETS srsran_support EXPORT srsran_export)

if (Backward_FOUND AND BACKWARD_HAS_EXTERNAL_LIBRARIES)
    set_source_files_properties(backtrace.cpp APPEND PROPERTIES COMPILE_DEFINITIONS "HAVE_BACKWARD")
    set_source_files_properties(backtrace.cpp APPEND PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/external)
endif (Backward_FOUND AND BACKWARD_HAS_EXTERNAL_LIBRARIES)