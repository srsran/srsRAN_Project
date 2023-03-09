#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES mac_ctrl/mac_configuration_helpers.cpp mac_config/mac_cell_group_config_factory.cpp)
add_library(mac_configuration_helpers STATIC ${SOURCES})
target_link_libraries(mac_configuration_helpers srsran_support srsran_ran)
set(SOURCES mac_factory.cpp
            mac_impl.cpp
            mac_ctrl/mac_controller.cpp
            mac_dl/mac_dl_processor.cpp
            mac_dl/mac_cell_processor.cpp
            mac_dl/rar_pdu_assembler.cpp
            mac_dl/dl_sch_pdu_assembler.cpp
            mac_dl/paging_pdu_assembler.cpp
            mac_dl/ssb_assembler.cpp
            mac_dl/mac_dl_ue_manager.cpp
            mac_ul/mac_ul_sch_pdu.cpp
            mac_ul/ul_bsr.cpp
            mac_ul/pdu_rx_handler.cpp)

add_library(srsran_mac STATIC ${SOURCES})
target_link_libraries(srsran_mac srsran_sched mac_configuration_helpers srsran_ran srsran_pcap rrc_nr_asn1)
