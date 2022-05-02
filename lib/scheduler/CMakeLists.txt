#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES sched_prb.cpp sched_harq.cpp sched.cpp sched_ssb.cpp cell/cell_configuration.cpp cell/resource_grid.cpp
        cell/ra_scheduler.cpp event_manager.cpp ue/ue.cpp scheduler_factory.cpp cell/pdcch_scheduler.cpp)

add_library(srsgnb_sched STATIC ${SOURCES})
target_link_libraries(srsgnb_sched srsgnb_ran rrc_nr_asn1_helpers)
