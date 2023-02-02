#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES serving_cell_config_factory.cpp
        logical_channel_config_factory.cpp
        sched_cell_config_helpers.cpp
        scheduler_cell_config_validator.cpp
        scheduler_ue_config_validator.cpp)
add_library(sched_config STATIC ${SOURCES})
target_link_libraries(sched_config srsgnb_ran)
