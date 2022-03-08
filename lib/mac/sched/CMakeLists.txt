
set(SOURCES sched_prb.cc sched_harq.cc sched_ssb.cc cell/cell_configuration.cc sched.cc)

add_library(srsgnb_sched STATIC ${SOURCES})
