#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES mac_ctrl/mac_configuration_helpers.cpp)
add_library(mac_configuration_helpers STATIC ${SOURCES})
target_link_libraries(mac_configuration_helpers srsgnb_support srsgnb_ran rrc_nr_asn1)

set(SOURCES mac_factory.cpp mac_impl.cpp mac_ctrl/mac_controller.cpp
        mac_dl/mac_dl_processor.cpp mac_dl/mac_cell_processor.cpp
        mac_ul/mac_ul_sch_pdu.cpp mac_ul/ul_bsr.cpp mac_ul/pdu_rx_handler.cpp mac_dl/ssb_assembler.cpp)

add_library(srsgnb_mac STATIC ${SOURCES})
target_link_libraries(srsgnb_mac srsgnb_sched mac_configuration_helpers srsgnb_ran)
