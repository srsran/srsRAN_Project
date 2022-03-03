
set(SOURCES sched_prb.cc sched_harq.cc sched_signalling.h sched_signalling.cc)

add_library(srsgnb_sched STATIC ${SOURCES})