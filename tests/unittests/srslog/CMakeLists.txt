#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(srslog_backend_throughput benchmarks/backend_throughput.cpp)
target_include_directories(srslog_backend_throughput PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srslog_backend_throughput srslog)

add_executable(srslog_frontend_latency benchmarks/frontend_latency.cpp)
target_include_directories(srslog_frontend_latency PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srslog_frontend_latency srslog)

add_executable(srslog_test srslog_test.cpp)
target_include_directories(srslog_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srslog_test srslog)
add_test(srslog_test srslog_test)

add_executable(log_channel_test log_channel_test.cpp)
target_include_directories(log_channel_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(log_channel_test srslog)
add_test(log_channel_test log_channel_test)

add_executable(log_backend_test log_backend_test.cpp)
target_include_directories(log_backend_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(log_backend_test srslog)
add_test(log_backend_test log_backend_test)

add_executable(logger_test logger_test.cpp)
target_include_directories(logger_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(logger_test srslog)
add_test(logger_test logger_test)

add_executable(any_test any_test.cpp)
target_include_directories(any_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(any_test srslog)
add_test(any_test any_test)

add_executable(file_sink_test file_sink_test.cpp)
target_include_directories(file_sink_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(file_sink_test srslog)
add_test(file_sink_test file_sink_test)

add_executable(syslog_sink_test syslog_sink_test.cpp)
target_include_directories(syslog_sink_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(syslog_sink_test srslog)

add_executable(file_utils_test file_utils_test.cpp)
target_include_directories(file_utils_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(file_utils_test srslog)
add_test(file_utils_test file_utils_test)

add_executable(tracer_test event_trace_test.cpp)
target_include_directories(tracer_test PRIVATE ${CMAKE_SOURCE_DIR})
add_definitions(-DENABLE_SRSLOG_EVENT_TRACE)
target_link_libraries(tracer_test srslog)
add_test(tracer_test tracer_test)

add_executable(text_formatter_test text_formatter_test.cpp)
target_include_directories(text_formatter_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(text_formatter_test srslog)
add_test(text_formatter_test text_formatter_test)

add_executable(json_formatter_test json_formatter_test.cpp)
target_include_directories(json_formatter_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(json_formatter_test srslog)
add_test(json_formatter_test json_formatter_test)

add_executable(context_test context_test.cpp)
target_include_directories(context_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(context_test srslog)
add_test(context_test context_test)
