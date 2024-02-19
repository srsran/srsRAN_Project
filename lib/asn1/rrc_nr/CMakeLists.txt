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

# RRC NR ASN1
add_library(rrc_nr_asn1 STATIC
        common.cpp
        serving_cell.cpp
        ue_cap.cpp
        bwp_cfg.cpp
        sys_info.cpp
        radio_bearer_cfg.cpp
        dl_ccch_msg_ies.cpp
        dl_dcch_msg_ies.cpp
        ul_ccch_msg_ies.cpp
        ul_dcch_msg_ies.cpp
        pcch_msg_ies.cpp
        dl_dcch_msg.cpp
        ul_dcch_msg.cpp
        nr_ue_variables.cpp
        pc5_rrc_definitions.cpp
        nr_sidelink_preconf.cpp
        cell_group_config.cpp
        bcch_bch_msg.cpp
        bcch_dl_sch_msg.cpp
        pcch_msg.cpp
        ul_ccch_msg.cpp
        ul_ccch1_msg.cpp
        dl_ccch_msg.cpp
        rrc_nr.cpp)
target_compile_options(rrc_nr_asn1 PRIVATE -Os -fno-exceptions)
target_link_libraries(rrc_nr_asn1 asn1_utils)
