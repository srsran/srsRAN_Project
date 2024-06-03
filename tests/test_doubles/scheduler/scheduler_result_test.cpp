/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_result_test.h"

using namespace srsran;

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
