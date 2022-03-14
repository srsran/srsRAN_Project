
add_subdirectory(sched)

set(SOURCES mac_factory.cc mac_impl.cc mac_ctrl/mac_ctrl_component.cc mac_ul/mac_ul_sch_pdu.cc)

add_library(srsgnb_mac STATIC ${SOURCES})
target_link_libraries(srsgnb_mac srsgnb_sched)
