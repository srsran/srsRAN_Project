#
# Copyright 2021-2024 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

set(SOURCES config/mac_cell_group_config_factory.cpp config/mac_config_helpers.cpp)
add_library(mac_configuration_helpers STATIC ${SOURCES})
target_link_libraries(mac_configuration_helpers srsran_support srsran_ran)
set(SOURCES mac_factory.cpp
            mac_impl.cpp
            mac_ctrl/mac_controller.cpp
            mac_ctrl/ue_creation_procedure.cpp
            mac_ctrl/ue_reconfiguration_procedure.cpp
            mac_ctrl/ue_delete_procedure.cpp
            mac_dl/mac_dl_processor.cpp
            mac_dl/mac_cell_processor.cpp
            mac_dl/rar_pdu_assembler.cpp
            mac_dl/dl_sch_pdu_assembler.cpp
            mac_dl/paging_pdu_assembler.cpp
            mac_dl/ssb_assembler.cpp
            mac_dl/mac_dl_ue_manager.cpp
            mac_ul/mac_ul_sch_pdu.cpp
            mac_ul/ul_bsr.cpp
            mac_ul/pdu_rx_handler.cpp
            mac_ul/mac_ul_ue_manager.cpp
            mac_sched/srsran_scheduler_adapter.cpp
            mac_sched/uci_cell_decoder.cpp)

add_library(srsran_mac STATIC ${SOURCES})
target_link_libraries(srsran_mac
        srsran_sched
        mac_configuration_helpers
        srsran_ran
        srsran_pcap
        rrc_nr_asn1
        srsran_instrumentation)
