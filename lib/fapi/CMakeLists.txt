#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(pdu_validators)

set(SOURCES
        message_loggers.cpp
        message_validators.cpp
        logging_decorator_factories.cpp
        logging_slot_data_notifier_decorator.cpp
        logging_slot_gateway_decorator.cpp
        logging_slot_time_notifier_decorator.cpp)

add_library(srsran_fapi STATIC ${SOURCES})
target_link_libraries(srsran_fapi fapi_pdu_validators srslog)
