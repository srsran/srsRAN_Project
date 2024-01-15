/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/mac/mac_cell_result.h"

namespace srsran {
namespace unittests {

/// Helper struct to manage the pointer life cycle of a SIB PDU.
struct sib_information_test_helper {
  bwp_configuration     bwp_cfg;
  coreset_configuration coreset_cfg;
  sib_information       pdu;
};

/// Builds and returns a valid SIB1 information PDU.
sib_information_test_helper build_valid_sib1_information_pdu();

/// Helper struct to manage the pointer life cycle of a RAR PDU.
struct rar_information_test_helper {
  bwp_configuration     bwp_cfg;
  coreset_configuration coreset_cfg;
  rar_information       pdu;
};

/// Builds and returns a valid RAR information PDU.
rar_information_test_helper build_valid_rar_information_pdu();

/// Helper struct to manage the pointer life cycle of a DL paging allocation PDU.
struct dl_paging_allocation_test_helper {
  bwp_configuration     bwp_cfg;
  coreset_configuration coreset_cfg;
  dl_paging_allocation  pdu;
};

/// Builds and returns a valid DL paging PDU.
dl_paging_allocation_test_helper build_valid_dl_paging_pdu();

/// Helper struct to manage the pointer life cycle of a DL message allocation PDU.
struct dl_msg_allocation_test_helper {
  bwp_configuration     bwp_cfg;
  coreset_configuration coreset_cfg;
  dl_msg_alloc          pdu;
};

/// Builds and returns a valid DL message allocation PDU.
dl_msg_allocation_test_helper build_valid_dl_msg_alloc_pdu();

/// Builds and returns a valid MAC SSB PDU.
dl_ssb_pdu build_valid_dl_ssb_pdu();

/// Helper struct to manage the pointer life cycle of a MAC DL scheduling result.
struct mac_dl_sched_result_test_helper {
  std::array<bwp_configuration, 2>     bwp_cfg;
  std::array<coreset_configuration, 2> coreset_cfg;
  sib_information_test_helper          sib1;
  dl_sched_result                      sched_result;
  mac_dl_sched_result                  result;
};

/// Builds and returns a valid MAC data result.
mac_dl_data_result build_valid_mac_data_result();

/// Builds and returns a valid PRACH occassion.
prach_occasion_info build_valid_prach_occassion();

/// Helper struct to manage the pointer life cycle of a UL scheduling info.
struct ul_sched_info_test_helper {
  bwp_configuration bwp_cfg;
  ul_sched_info     info;
};

/// Builds and returns a valid PUSCH PDU.
ul_sched_info_test_helper build_valid_pusch_pdu();

/// Helper struct to manage the pointer life cycle of a PUCCH info.
struct pucch_info_test_helper {
  bwp_configuration bwp_cfg;
  pucch_info        info;
};

/// Build and returns a valid PUCCH format 1 PDU.
pucch_info_test_helper build_valid_pucch_format_1_pdu();

/// Build and returns a valid PUCCH format 2 PDU.
pucch_info_test_helper build_valid_pucch_format_2_pdu();

/// Builds and returns a valid MAC DL sched result.
mac_dl_sched_result_test_helper build_valid_mac_dl_sched_result();

/// Helper struct to manage the pointer life cycle of a MAC UL scheduling result.
struct mac_ul_sched_result_test_helper {
  pucch_info_test_helper    pucch_f1;
  pucch_info_test_helper    pucch_f2;
  ul_sched_info_test_helper pusch;
  ul_sched_result           sched_result;
  mac_ul_sched_result       result;
};

/// Builds and returns a valid MAC UL sched result.
mac_ul_sched_result_test_helper build_valid_mac_ul_sched_result();

} // namespace unittests
} // namespace srsran
