
add_subdirectory(sched)

set(SOURCES mac_factory.cc mac_impl.cc mac_ctrl/mac_ctrl_component.cc mac_dl/mac_dl_processor.cc
            mac_ul/mac_ul_sch_pdu.cc mac_ul/ul_bsr.cc mac_ul/pdu_rx_handler.cc)

add_library(srsgnb_mac STATIC ${SOURCES})
target_link_libraries(srsgnb_mac srsgnb_sched)
