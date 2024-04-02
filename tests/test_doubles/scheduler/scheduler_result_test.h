/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

inline const pdcch_dl_information* find_ue_dl_pdcch(rnti_t rnti, const dl_sched_result& res)
{
  auto* it = std::find_if(res.dl_pdcchs.begin(), res.dl_pdcchs.end(), [rnti](const pdcch_dl_information& pdcch) {
    return pdcch.ctx.rnti == rnti;
  });
  return it != res.dl_pdcchs.end() ? &*it : nullptr;
}

inline const pdcch_ul_information* find_ue_ul_pdcch(rnti_t rnti, const dl_sched_result& res)
{
  auto* it = std::find_if(res.ul_pdcchs.begin(), res.ul_pdcchs.end(), [rnti](const pdcch_ul_information& pdcch) {
    return pdcch.ctx.rnti == rnti;
  });
  return it != res.ul_pdcchs.end() ? &*it : nullptr;
}

inline const ul_sched_info* find_ue_pusch(rnti_t rnti, span<const ul_sched_info> ulgrants)
{
  auto it = std::find_if(
      ulgrants.begin(), ulgrants.end(), [rnti](const auto& pusch) { return pusch.pusch_cfg.rnti == rnti; });
  return it != ulgrants.end() ? &*it : nullptr;
}

inline const ul_sched_info* find_ue_pusch(rnti_t rnti, const ul_sched_result& res)
{
  return find_ue_pusch(rnti, res.puschs);
}

inline const pucch_info* find_ue_pucch(rnti_t rnti, span<const pucch_info> pucchs)
{
  auto it = std::find_if(pucchs.begin(), pucchs.end(), [rnti](const pucch_info& pucch) { return pucch.crnti == rnti; });
  return it != pucchs.end() ? &*it : nullptr;
}

inline const pucch_info* find_ue_pucch_with_harq_ack(rnti_t rnti, span<const pucch_info> pucchs)
{
  const pucch_info* pucch = find_ue_pucch(rnti, pucchs);
  if (pucch != nullptr) {
    if (pucch->format == pucch_format::FORMAT_1 and pucch->format_1.harq_ack_nof_bits > 0) {
      return pucch;
    }
    if (pucch->format == pucch_format::FORMAT_2 and pucch->format_2.harq_ack_nof_bits > 0) {
      return pucch;
    }
  }
  return nullptr;
}

inline const pucch_info* find_ue_pucch_with_sr(rnti_t rnti, span<const pucch_info> pucchs)
{
  const pucch_info* pucch = find_ue_pucch(rnti, pucchs);
  if (pucch != nullptr) {
    if (pucch->format == pucch_format::FORMAT_1 and pucch->format_1.sr_bits != sr_nof_bits::no_sr) {
      return pucch;
    }
    if (pucch->format == pucch_format::FORMAT_2 and pucch->format_2.sr_bits != sr_nof_bits::no_sr) {
      return pucch;
    }
  }
  return nullptr;
}

inline const pucch_info* find_ue_pucch_with_csi(rnti_t rnti, span<const pucch_info> pucchs)
{
  const pucch_info* pucch = find_ue_pucch(rnti, pucchs);
  if (pucch != nullptr) {
    if (pucch->format == pucch_format::FORMAT_2 and pucch->format_2.csi_part1_bits > 0) {
      return pucch;
    }
  }
  return nullptr;
}

} // namespace srsran