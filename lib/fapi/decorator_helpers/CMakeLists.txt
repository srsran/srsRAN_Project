#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        slot_data_message_notifier_dummy.cpp
        slot_error_notifier_dummy.cpp
        slot_last_message_notifier_dummy.cpp
        slot_time_notifier_dummy.cpp
        slot_data_message_notifier_dispatcher.cpp
        slot_error_notifier_dispatcher.cpp
        slot_last_message_notifier_dispatcher.cpp
)

add_library(srsran_fapi_decorator_helpers STATIC ${SOURCES})
