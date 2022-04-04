
set(SOURCES sched_prb.cpp sched_harq.cpp sched.cpp sched_ssb.cpp cell/cell_configuration.cpp cell/resource_grid.cpp
            cell/phy_helpers.cpp cell/ra_sched.cpp event_manager.cpp ue/ue.cpp)

add_library(srsgnb_sched STATIC ${SOURCES})
target_link_libraries(srsgnb_sched rrc_nr_asn1_helpers)
