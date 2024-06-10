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

#include "scheduler_result_test.h"

using namespace srsran;

const dl_msg_alloc* srsran::find_ue_pdsch_with_lcid(rnti_t rnti, lcid_dl_sch_t lcid, span<const dl_msg_alloc> dlgrants)
{
  auto it = std::find_if(dlgrants.begin(), dlgrants.end(), [rnti, lcid](const dl_msg_alloc& pdsch) {
    return pdsch.pdsch_cfg.rnti == rnti and std::any_of(pdsch.tb_list[0].lc_chs_to_sched.begin(),
                                                        pdsch.tb_list[0].lc_chs_to_sched.end(),
                                                        [lcid](const dl_msg_lc_info& t) { return t.lcid == lcid; });
  });
  return it != dlgrants.end() ? &*it : nullptr;
}

const dl_msg_alloc* srsran::find_ue_pdsch_with_lcid(rnti_t rnti, lcid_t lcid, span<const dl_msg_alloc> dlgrants)
{
  return find_ue_pdsch_with_lcid(rnti, lcid_dl_sch_t{lcid}, dlgrants);
}

const csi_report_configuration* srsran::find_ue_uci_with_csi(rnti_t rnti, const ul_sched_result& ul_res)
{
  if (ul_res.nof_ul_symbols == 0) {
    return nullptr;
  }
  const pucch_info* pucch = find_ue_pucch_with_csi(rnti, ul_res.pucchs);
  if (pucch != nullptr) {
    return &pucch->csi_rep_cfg.value();
  }
  for (const ul_sched_info& ul : ul_res.puschs) {
    if (ul.pusch_cfg.rnti == rnti and ul.uci.has_value() and ul.uci.value().csi.has_value()) {
      return &ul.uci.value().csi.value().csi_rep_cfg;
    }
  }
  return nullptr;
}
