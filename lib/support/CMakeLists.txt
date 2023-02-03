#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

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
        config_yaml.cpp)

add_library(srsgnb_support STATIC ${SOURCES})
target_link_libraries(srsgnb_support srsgnb_io_broker ${CMAKE_THREAD_LIBS_INIT} ${YAMLCPP_LIBRARY} srslog)

install(TARGETS srsgnb_support EXPORT srsgnb_export)

if (Backward_FOUND AND BACKWARD_HAS_EXTERNAL_LIBRARIES)
    set_source_files_properties(backtrace.cpp APPEND PROPERTIES COMPILE_DEFINITIONS "HAVE_BACKWARD")
    set_source_files_properties(backtrace.cpp APPEND PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/external)
endif (Backward_FOUND AND BACKWARD_HAS_EXTERNAL_LIBRARIES)
