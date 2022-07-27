/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lib/scheduler/cell/resource_grid.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

struct test_grant_info {
  enum type_t { SSB, DL_PDCCH, UL_PDCCH, SIB, RAR, UE } type;
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
grant_info get_pdsch_grant_info(const rar_information& rar);

/// Extract PDSCH grant info of UE DL allocation.
///
/// \param ue_grant UE PDSCH grant parameters.
/// \return Parameters of the grant.
grant_info get_pdsch_grant_info(const dl_msg_alloc& ue_grant);

/// \brief Creates a list with the grants allocated in a cell DL resource grid. This includes PDCCHs, PDSCHs and SSB.
std::vector<test_grant_info> get_dl_grants(const cell_configuration& cell_cfg, const dl_sched_result& dl_res);

} // namespace srsgnb
