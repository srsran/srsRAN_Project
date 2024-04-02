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

#include "tests/test_doubles/scheduler/scheduler_result_test.h"
#include "srsran/scheduler/scheduler_feedback_handler.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

inline const pdcch_dl_information* find_ue_dl_pdcch(rnti_t rnti, const sched_result& res)
{
  return find_ue_dl_pdcch(rnti, res.dl);
}

inline const pdcch_ul_information* find_ue_ul_pdcch(rnti_t rnti, const sched_result& res)
{
  return find_ue_ul_pdcch(rnti, res.dl);
}

inline const ul_sched_info* find_ue_pusch(rnti_t rnti, const sched_result& res)
{
  return find_ue_pusch(rnti, res.ul.puschs);
}

inline const ul_sched_info* find_ue_pusch_with_harq_ack(rnti_t rnti, const sched_result& res)
{
  const ul_sched_info* pusch = find_ue_pusch(rnti, res);
  if (pusch != nullptr) {
    if (pusch->uci.has_value() and pusch->uci->harq.has_value() and pusch->uci->harq->harq_ack_nof_bits > 0) {
      return pusch;
    }
  }
  return nullptr;
}

inline const dl_msg_alloc* find_ue_pdsch(rnti_t rnti, span<const dl_msg_alloc> dlgrants)
{
  auto it = std::find_if(
      dlgrants.begin(), dlgrants.end(), [rnti](const auto& pusch) { return pusch.pdsch_cfg.rnti == rnti; });
  return it != dlgrants.end() ? &*it : nullptr;
}

inline const dl_msg_alloc* find_ue_pdsch(rnti_t rnti, const sched_result& res)
{
  return find_ue_pdsch(rnti, res.dl.ue_grants);
}

inline const pucch_info* find_ue_pucch(rnti_t rnti, const sched_result& res)
{
  return find_ue_pucch(rnti, res.ul.pucchs);
}

inline const pucch_info* find_ue_pucch_with_harq_ack(rnti_t rnti, const sched_result& res)
{
  return find_ue_pucch_with_harq_ack(rnti, res.ul.pucchs);
}

inline uci_indication::uci_pdu create_uci_pdu_with_harq_ack(du_ue_index_t ue_index, const pucch_info& pucch_pdu)
{
  uci_indication::uci_pdu pdu;
  pdu.crnti    = pucch_pdu.crnti;
  pdu.ue_index = ue_index;
  switch (pucch_pdu.format) {
    case pucch_format::FORMAT_1: {
      uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu f0_1_pdu{};
      f0_1_pdu.harqs.resize(pucch_pdu.format_1.harq_ack_nof_bits, mac_harq_ack_report_status::ack);
      pdu.pdu = f0_1_pdu;
    } break;
    case pucch_format::FORMAT_2: {
      uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu f2_pdu{};
      f2_pdu.harqs.resize(pucch_pdu.format_2.harq_ack_nof_bits, mac_harq_ack_report_status::ack);
      pdu.pdu = f2_pdu;
    } break;
    default:
      srsran_terminate("Format {} not supported", pucch_pdu.format);
      break;
  }
  return pdu;
}

} // namespace srsran
