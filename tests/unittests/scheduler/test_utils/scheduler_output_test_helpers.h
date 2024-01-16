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

#include "lib/scheduler/cell/resource_grid.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

/// Parameters of a grant allocation in the cell resource grid.

struct test_grant_info {
  enum type_t { SSB, DL_PDCCH, UL_PDCCH, SIB, RAR, UE_DL, UE_UL, PUCCH, PRACH, PAGING } type;
  rnti_t     rnti;
  grant_info grant;
};

/// Extract PDCCH grant info of DL PDCCH allocation.
///
/// \param pci Cell PCI.
/// \param pdcch DL PDCCH allocation parameters.
/// \return Parameters of the grant.
std::vector<grant_info> get_pdcch_grant_info(pci_t pci, const pdcch_dl_information& pdcch);

/// Extract PDCCH grant info of UL PDCCH allocation.
///
/// \param pdcch UL PDCCH allocation parameters.
/// \return Parameters of the grant.
std::vector<grant_info> get_pdcch_grant_info(const pdcch_ul_information& pdcch);

/// Extract PDSCH grant info of SIB allocation.
///
/// \param bwp_cfg Configuration of the DL BWP used for SIB allocation.
/// \param sib SIB allocation parameters.
/// \return Parameters of the grant.
grant_info get_pdsch_grant_info(const bwp_downlink_common& bwp_cfg, const sib_information& sib);

/// Extract PDSCH grant info of RAR allocation.
///
/// \param rar RAR allocation parameters.
/// \return Parameters of the grant.
grant_info get_pdsch_grant_info(const bwp_downlink_common& bwp_cfg, const rar_information& rar);

/// Extract PDSCH grant info of Paging allocation.
///
/// \param bwp_cfg Initial DL BWP with CORESET#0 configuration.
/// \param pg Paging allocation parameters.
/// \return Parameters of the grant.
grant_info get_pdsch_grant_info(const bwp_downlink_common& bwp_cfg, const dl_paging_allocation& pg);

/// Extract PDSCH grant info of UE DL allocation.
///
/// \param ue_grant UE PDSCH grant parameters.
/// \return Parameters of the grant.
grant_info get_pdsch_grant_info(const bwp_downlink_common& bwp_cfg, const dl_msg_alloc& ue_grant);

/// \brief Creates a list with the grants allocated in a cell DL resource grid. This includes PDCCHs, PDSCHs and SSB.
std::vector<test_grant_info> get_dl_grants(const cell_configuration& cell_cfg, const dl_sched_result& dl_res);

/// \brief Creates a list with the grants allocated in a cell UL resource grid. This includes PUSCHs, PUCCHs and PRACH
/// PDUs.
std::vector<test_grant_info> get_ul_grants(const cell_configuration& cell_cfg, const ul_sched_result& ul_res);

} // namespace srsran
