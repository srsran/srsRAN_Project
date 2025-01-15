#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        logging_decorator_impl.cpp
        logging_slot_data_notifier_decorator.cpp
        logging_slot_error_notifier_decorator.cpp
        logging_slot_gateway_decorator.cpp
        logging_slot_time_notifier_decorator.cpp
        message_loggers.cpp)

add_library(srsran_fapi_loggers STATIC ${SOURCES})
target_include_directories(srsran_fapi_loggers PRIVATE ${CMAKE_SOURCE_DIR}/lib/fapi)
target_link_libraries(srsran_fapi_loggers srslog srsran_fapi_decorator_helpers)
