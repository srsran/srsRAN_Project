#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(validators)

set(SOURCES
        message_loggers.cpp
        logging_decorator_factories.cpp
        logging_slot_data_notifier_decorator.cpp
        logging_slot_gateway_decorator.cpp
        logging_slot_time_noitifier_decorator.cpp)

add_library(srsgnb_fapi_message_loggers STATIC ${SOURCES})
target_link_libraries(srsgnb_fapi_message_loggers srslog)
