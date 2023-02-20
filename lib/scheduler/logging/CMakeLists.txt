#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES scheduler_result_logger.cpp scheduler_metric_handler.cpp scheduler_event_logger.cpp)
add_library(scheduler_logger STATIC ${SOURCES})
target_link_libraries(scheduler_logger srsgnb_ran srslog)
