
set(SOURCES sched_prb.cc sched_harq.cc sched.cc sched_ssb.cc cell/cell_configuration.cc cell/resource_grid.cc)

add_library(srsgnb_sched STATIC ${SOURCES})
target_link_libraries(srsgnb_sched rrc_nr_asn1)
