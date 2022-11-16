/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue.h"
#include "../support/dmrs_helpers.h"
#include "../support/prbs_calculator.h"

using namespace srsgnb;

unsigned ue_cell::required_dl_prbs(unsigned time_resource, unsigned pending_bytes) const
{
  static const unsigned        nof_oh_prb = 0; // TODO
  static const unsigned        nof_layers = 1;
  static const pdsch_mcs_table mcs_table  = pdsch_mcs_table::qam64;
  const cell_configuration&    cell_cfg   = cfg().cell_cfg_common;

  if (pending_bytes * 8U > 3824) {
    // TODO: support get_nof_prbs higher than 3824 bits.
    return cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs.length();
  }

  sch_mcs_index       mcs        = expert_cfg.fixed_dl_mcs.value(); // TODO: Support dynamic MCS.
  sch_mcs_description mcs_config = pdsch_mcs_get_config(mcs_table, mcs);

  dmrs_information dmrs_info = make_dmrs_info_common(
      cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common, time_resource, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);
  pdsch_prbs_tbs prbs_tbs = get_nof_prbs(prbs_calculator_pdsch_config{
      pending_bytes,
      (unsigned)cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[time_resource].symbols.length(),
      calculate_nof_dmrs_per_rb(dmrs_info),
      nof_oh_prb,
      mcs_config.modulation,
      mcs_config.target_code_rate / 1024.0F,
      nof_layers});
  return prbs_tbs.nof_prbs;
}

unsigned ue_cell::required_ul_prbs(unsigned time_resource, unsigned pending_bytes) const
{
  static const unsigned        nof_oh_prb = 0; // TODO
  static const unsigned        nof_layers = 1;
  static const pusch_mcs_table mcs_table  = pusch_mcs_table::qam64;
  const cell_configuration&    cell_cfg   = cfg().cell_cfg_common;

  if (pending_bytes * 8U > 3824) {
    // TODO: support get_nof_prbs higher than 3824 bits.
    return cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
  }

  sch_mcs_index       mcs        = expert_cfg.fixed_ul_mcs.value(); // TODO: Support dynamic MCS.
  sch_mcs_description mcs_config = pusch_mcs_get_config(mcs_table, mcs, false);

  dmrs_information dmrs_info = make_dmrs_info_common(
      *cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common, time_resource, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);
  pdsch_prbs_tbs prbs_tbs = get_nof_prbs(prbs_calculator_pdsch_config{
      pending_bytes,
      (unsigned)cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[time_resource]
          .symbols.length(),
      calculate_nof_dmrs_per_rb(dmrs_info),
      nof_oh_prb,
      mcs_config.modulation,
      mcs_config.target_code_rate / 1024.0F,
      nof_layers});
  return prbs_tbs.nof_prbs;
}

void ue::handle_reconfiguration_request(const sched_ue_reconfiguration_message& msg)
{
  // TODO.
}

unsigned ue::pending_dl_newtx_bytes() const
{
  unsigned pending_bytes = dl_lc_ch_mgr.pending_bytes(LCID_SRB0);

  if (pending_bytes > 0) {
    // In case SRB0 has data, only allocate SRB0 and CEs.
    return pending_bytes + dl_lc_ch_mgr.pending_ce_bytes();
  }

  return dl_lc_ch_mgr.pending_bytes();
}

unsigned ue::pending_ul_newtx_bytes() const
{
  constexpr static unsigned SR_GRANT_BYTES = 512;

  // Sum the last BSRs.
  unsigned pending_bytes = ul_lc_ch_mgr.pending_bytes();

  // Subtract the bytes already allocated in UL HARQs.
  for (const ue_cell* ue_cc : ue_cells) {
    if (pending_bytes == 0) {
      break;
    }
    unsigned harq_bytes = 0;
    for (unsigned i = 0; i != ue_cc->harqs.nof_ul_harqs(); ++i) {
      const ul_harq_process& h_ul = ue_cc->harqs.ul_harq(i);
      if (not h_ul.empty()) {
        harq_bytes += h_ul.last_tx_params().tbs_bytes;
      }
    }
    pending_bytes -= std::min(pending_bytes, harq_bytes);
  }

  // If there are no pending bytes, check if a SR is pending.
  return pending_bytes > 0 ? pending_bytes : (ul_lc_ch_mgr.has_pending_sr() ? SR_GRANT_BYTES : 0);
}

unsigned ue::build_dl_transport_block_info(dl_msg_tb_info& tb_info, unsigned tb_size_bytes)
{
  unsigned total_subpdu_bytes = 0;
  total_subpdu_bytes += allocate_mac_ces(tb_info, dl_lc_ch_mgr, tb_size_bytes);
  total_subpdu_bytes += allocate_mac_sdus(tb_info, dl_lc_ch_mgr, tb_size_bytes - total_subpdu_bytes);
  return total_subpdu_bytes;
}
