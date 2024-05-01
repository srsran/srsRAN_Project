#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        buffered_decorator_factories.cpp
        buffered_decorator_impl.cpp
        buffered_slot_gateway_impl.cpp
        buffered_slot_gateway_task_dispatcher.cpp
        buffered_slot_time_notifier_decorator.cpp)

add_library(srsran_fapi_message_buffering STATIC ${SOURCES})
