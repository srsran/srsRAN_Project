/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ra_scheduler.h"
#include "../logging/scheduler_event_logger.h"
#include "../pdcch_scheduling/pdcch_config_helpers.h"
#include "../pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "../support/dci_builder.h"
#include "../support/dmrs_helpers.h"
#include "../support/pdsch/pdsch_default_time_allocation.h"
#include "../support/pdsch/pdsch_resource_allocation.h"
#include "../support/sch_pdu_builder.h"
#include "../support/tbs_calculator.h"
#include "srsran/ran/resource_allocation/resource_allocation_frequency.h"
#include "srsran/support/compiler.h"

using namespace srsran;

unsigned srsran::get_msg3_delay(const pusch_time_domain_resource_allocation& pusch_td_res_alloc,
                                subcarrier_spacing                           pusch_scs)
{
  // In TS 38.214, Table 6.1.2.1.1-5, Delta is only defined for PUSCH SCS within [kHz15, kHz120kHz].
  srsran_sanity_check(to_numerology_value(pusch_scs) <= to_numerology_value(subcarrier_spacing::kHz120),
                      "PUSCH subcarrier spacing not supported for MSG3 delay");

  // The array represents Table 6.1.2.1.1-5, in TS 38.214.
  static const std::array<uint8_t, 4> DELTAS{2, 3, 4, 6};

  // The MSG3 slot is defined as MSG3_slot = floor( n * (2^*(mu_PUSCH) ) / (2^*(mu_PDCCH) ) ) + k2 + Delta.
  // Given the assumption mu_PUSCH == mu_PDCCH, MSG3_delay simplifies to MSG3_delay =  k2 + Delta
  // [TS 38.214, Section 6.1.2.1 and 6.1.2.1.1].
  return static_cast<int>(pusch_td_res_alloc.k2 + DELTAS[to_numerology_value(pusch_scs)]);
}

uint16_t srsran::get_ra_rnti(unsigned slot_index, unsigned symbol_index, unsigned frequency_index, bool is_sul)
{
  // See 38.321, 5.1.3 - Random Access Preamble transmission.
  // RA-RNTI = 1 + s_id + 14 × t_id + 14 × 80 × f_id + 14 × 80 × 8 × ul_carrier_id.
  // s_id = index of the first OFDM symbol of the (first, for short formats) PRACH occasion (0 <= s_id < 14).
  // t_id = index of the first slot of the PRACH occasion in a system frame (0 <= t_id < 80); the numerology of
  // reference for t_id is 15kHz for long PRACH Formats, regardless of the SCS common; whereas, for short PRACH formats,
  // it coincides with SCS common (this can be inferred from Section 5.1.3, TS 38.321, and from Section 5.3.2,
  // TS 38.211).
  // f_id = index of the PRACH occation in the freq domain (0 <= f_id < 8).
  // ul_carrier_id = 0 for NUL and 1 for SUL carrier.
  const uint16_t ra_rnti =
      1U + symbol_index + 14U * slot_index + 14U * 80U * frequency_index + (14U * 80U * 8U * (is_sul ? 1U : 0U));
  return ra_rnti;
}

static vrb_interval msg3_crb_to_vrb(const cell_configuration& cell_cfg, crb_interval grant_crbs)
{
  return rb_helper::crb_to_vrb_ul_non_interleaved(grant_crbs,
                                                  cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start());
}

static crb_interval msg3_vrb_to_crb(const cell_configuration& cell_cfg, vrb_interval grant_vrbs)
{
  return rb_helper::vrb_to_crb_ul_non_interleaved(grant_vrbs,
                                                  cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ra_scheduler::ra_scheduler(const scheduler_ra_expert_config& sched_cfg_,
                           const cell_configuration&         cellcfg_,
                           pdcch_resource_allocator&         pdcch_sch_,
                           scheduler_event_logger&           ev_logger_) :
  sched_cfg(sched_cfg_),
  cell_cfg(cellcfg_),
  pdcch_sch(pdcch_sch_),
  ev_logger(ev_logger_),
  ra_win_nof_slots(cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.ra_resp_window),
  ra_crb_lims(
      pdsch_helper::get_ra_crb_limits_common(cell_cfg.dl_cfg_common.init_dl_bwp,
                                             cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id)),
  prach_format_is_long(is_long_preamble(
      prach_configuration_get(band_helper::get_freq_range(cell_cfg.band),
                              band_helper::get_duplex_mode(cell_cfg.band),
                              cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index)
          .format)),
  pending_msg3s(MAX_NOF_MSG3)
{
  // Precompute RAR PDSCH and DCI PDUs.
  precompute_rar_fields();

  // Precompute Msg3 PUSCH and DCI PDUs.
  precompute_msg3_pdus();
}

void ra_scheduler::precompute_rar_fields()
{
  // RAR payload size in bytes as per TS38.321, 6.1.5 and 6.2.3.
  static const unsigned rar_payload_size_bytes = 7, rar_subheader_size_bytes = 1;
  // As per TS 38.214, Section 5.1.3.2, nof_oh_prb = 0 if PDSCH is scheduled by PDCCH with a CRC scrambled by RA-RNTI.
  static const unsigned nof_oh_prb = 0;
  static const unsigned nof_layers = 1;

  const span<const pdsch_time_domain_resource_allocation> pdsch_td_list =
      get_ra_rnti_pdsch_time_domain_list(cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common,
                                         cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.cp,
                                         cell_cfg.dmrs_typeA_pos);

  // Cache PDSCH DM-RS information and RAR required TBS and number of PRBs.
  rar_data.resize(pdsch_td_list.size());
  rar_mcs_config = pdsch_mcs_get_config(pdsch_mcs_table::qam64, sched_cfg.rar_mcs_index);
  for (unsigned td_idx = 0; td_idx != rar_data.size(); ++td_idx) {
    // > Compute and cache DM-RS information for different PDSCH TD indexes.
    rar_data[td_idx].dmrs_info = make_dmrs_info_common(pdsch_td_list, td_idx, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);

    // > Compute and cache #PRBs and TBS information for different PDSCH TD indexes.
    const unsigned             nof_symb_sh = pdsch_td_list[td_idx].symbols.length();
    prbs_calculator_sch_config prbs_cfg{rar_payload_size_bytes + rar_subheader_size_bytes,
                                        nof_symb_sh,
                                        calculate_nof_dmrs_per_rb(rar_data[td_idx].dmrs_info),
                                        nof_oh_prb,
                                        rar_mcs_config,
                                        nof_layers};
    for (unsigned ngrant = 0; ngrant != MAX_RAR_PDUS_PER_SLOT; ++ngrant) {
      prbs_cfg.payload_size_bytes = (rar_payload_size_bytes + rar_subheader_size_bytes) * (ngrant + 1);
      rar_data[td_idx].prbs_tbs_per_nof_grants.push_back(get_nof_prbs(prbs_cfg));
    }
  }
}

void ra_scheduler::precompute_msg3_pdus()
{
  // Msg3 UL CCCH message size is up to 64bits (8 octets), and its subheader has 1 octet as per TS38.321.
  static constexpr unsigned max_msg3_sdu_payload_size_bytes = 8, msg3_subheader_size_bytes = 1;
  // As per TS 38.214, Section 5.1.3.2, nof_oh_prb = 0 if PDSCH is scheduled by PDCCH with a CRC scrambled by RA-RNTI.
  static const unsigned nof_oh_prb = 0;
  static const unsigned nof_layers = 1;

  msg3_mcs_config = pusch_mcs_get_config(pusch_mcs_table::qam64, sched_cfg.msg3_mcs_index, false);

  const auto& pusch_td_alloc_list = get_pusch_time_domain_resource_table(get_pusch_cfg());

  // One position per time resource.
  msg3_data.resize(pusch_td_alloc_list.size());

  for (unsigned i = 0; i != msg3_data.size(); ++i) {
    // Create a dummy HARQ used to fill DCI and PUSCH.
    harq_logger      dummy_harq_logger{logger, to_rnti(0x4601), cell_cfg.cell_index, false};
    ul_harq_process  dummy_h_ul(to_harq_id(0), dummy_harq_logger);
    const slot_point dummy_slot{to_numerology_value(get_ul_bwp_cfg().scs), 0};
    dummy_h_ul.new_tx(dummy_slot, sched_cfg.max_nof_msg3_harq_retxs);

    // Compute the required PRBs and TBS for Msg3.
    const pusch_config_params pusch_cfg = get_pusch_config_f0_0_tc_rnti(cell_cfg, pusch_td_alloc_list[i]);
    const sch_prbs_tbs        prbs_tbs =
        get_nof_prbs(prbs_calculator_sch_config{max_msg3_sdu_payload_size_bytes + msg3_subheader_size_bytes,
                                                (unsigned)pusch_td_alloc_list[i].symbols.length(),
                                                calculate_nof_dmrs_per_rb(pusch_cfg.dmrs),
                                                nof_oh_prb,
                                                msg3_mcs_config,
                                                nof_layers});

    // Generate DCI and PUSCH PDUs.
    static constexpr uint8_t msg3_rv = 0;
    build_dci_f0_0_tc_rnti(msg3_data[i].dci,
                           cell_cfg.dl_cfg_common.init_dl_bwp,
                           cell_cfg.ul_cfg_common.init_ul_bwp.generic_params,
                           crb_interval{0, prbs_tbs.nof_prbs},
                           i,
                           sched_cfg.msg3_mcs_index,
                           msg3_rv,
                           dummy_h_ul);

    // Note: RNTI will be overwritten later.
    build_pusch_f0_0_tc_rnti(msg3_data[i].pusch,
                             pusch_cfg,
                             prbs_tbs.tbs_bytes,
                             to_rnti(0x4601),
                             cell_cfg,
                             msg3_data[i].dci.tc_rnti_f0_0,
                             crb_interval{0, prbs_tbs.nof_prbs},
                             true);
  }
}

void ra_scheduler::handle_rach_indication(const rach_indication_message& msg)
{
  // Buffer detected RACHs to be handled in next slot.
  pending_rachs.push(msg);
}

void ra_scheduler::handle_rach_indication_impl(const rach_indication_message& msg)
{
  const static unsigned prach_duration = 1; // TODO: Take from config

  for (const auto& prach_occ : msg.occasions) {
    // As per Section 5.1.3, TS 38.321, and from Section 5.3.2, TS 38.211, slot_idx uses as the numerology of reference
    // 15kHz for long PRACH Formats (i.e, slot_idx = subframe index); whereas, for short PRACH formats, it uses the same
    // numerology as the SCS common (i.e, slot_idx = actual slot index within the frame).
    const unsigned slot_idx = prach_format_is_long ? msg.slot_rx.subframe_index() : msg.slot_rx.slot_index();
    const uint16_t ra_rnti  = get_ra_rnti(slot_idx, prach_occ.start_symbol, prach_occ.frequency_index);

    pending_rar_t* rar_req = nullptr;
    for (pending_rar_t& rar : pending_rars) {
      if (rar.ra_rnti == ra_rnti and rar.prach_slot_rx == msg.slot_rx) {
        rar_req = &rar;
        break;
      }
    }
    if (rar_req == nullptr) {
      // Create new pending RAR
      pending_rars.emplace_back();
      rar_req                = &pending_rars.back();
      rar_req->ra_rnti       = to_rnti(ra_rnti);
      rar_req->prach_slot_rx = msg.slot_rx;
    }

    // Set RAR window. First slot after PRACH with active DL slot represents the start of the RAR window.
    if (cell_cfg.tdd_cfg_common.has_value()) {
      // TDD case.
      const unsigned period = nof_slots_per_tdd_period(*cell_cfg.tdd_cfg_common);
      for (unsigned sl_idx = 0; sl_idx < period; ++sl_idx) {
        const slot_point sl_start = rar_req->prach_slot_rx + prach_duration + sl_idx;
        if (cell_cfg.is_dl_enabled(sl_start)) {
          rar_req->rar_window = {sl_start, sl_start + ra_win_nof_slots};
          break;
        }
      }
      srsran_sanity_check(rar_req->rar_window.length() != 0, "Invalid configuration");
    } else {
      // FDD case.
      rar_req->rar_window = {rar_req->prach_slot_rx + prach_duration,
                             rar_req->prach_slot_rx + prach_duration + ra_win_nof_slots};
    }

    for (const auto& prach_preamble : prach_occ.preambles) {
      // Log event.
      ev_logger.enqueue(scheduler_event_logger::prach_event{msg.slot_rx,
                                                            msg.cell_index,
                                                            prach_preamble.preamble_id,
                                                            to_rnti(ra_rnti),
                                                            prach_preamble.tc_rnti,
                                                            prach_preamble.time_advance.to_Ta(get_ul_bwp_cfg().scs)});

      // Check if TC-RNTI value to be scheduled is already under use
      if (not pending_msg3s[prach_preamble.tc_rnti % MAX_NOF_MSG3].harq.empty()) {
        logger.warning("PRACH ignored, as the allocated TC-RNTI=0x{:x} is already under use", prach_preamble.tc_rnti);
        continue;
      }

      // Store TC-RNTI of the preamble.
      rar_req->tc_rntis.emplace_back(prach_preamble.tc_rnti);

      // Store Msg3 to allocate.
      pending_msg3s[prach_preamble.tc_rnti % MAX_NOF_MSG3].preamble = prach_preamble;
      pending_msg3s[prach_preamble.tc_rnti % MAX_NOF_MSG3].msg3_harq_logger.set_rnti(prach_preamble.tc_rnti);
    }
  }
}

void ra_scheduler::handle_crc_indication(const ul_crc_indication& crc_ind)
{
  pending_crcs.push(crc_ind);
}

void ra_scheduler::handle_pending_crc_indications_impl(cell_resource_allocator& res_alloc)
{
  // Pop pending CRCs and process them.
  pending_crcs.slot_indication();
  const span<const ul_crc_indication> new_crc_inds = pending_crcs.get_events();

  for (const ul_crc_indication& crc_ind : new_crc_inds) {
    for (const ul_crc_pdu_indication& crc : crc_ind.crcs) {
      srsran_assert(crc.ue_index == INVALID_DU_UE_INDEX, "Msg3 HARQ CRCs cannot have a ueId assigned yet");
      auto& pending_msg3 = pending_msg3s[crc.rnti % MAX_NOF_MSG3];
      if (pending_msg3.preamble.tc_rnti != crc.rnti) {
        logger.warning("Invalid UL CRC, cell={}, rnti={:#x}, h_id={}. Cause: Inexistent rnti.",
                       cell_cfg.cell_index,
                       crc.rnti,
                       crc.harq_id);
        continue;
      }
      if (pending_msg3.harq.id != crc.harq_id) {
        logger.warning("Invalid UL CRC, cell={}, rnti={:#x}, h_id={}. Cause: HARQ-Ids do not match ({} != {})",
                       cell_cfg.cell_index,
                       crc.rnti,
                       crc.harq_id,
                       crc.harq_id,
                       pending_msg3.harq.id);
        continue;
      }
      pending_msg3.harq.crc_info(crc.tb_crc_success);
    }
  }

  // Allocate pending Msg3 retransmissions.
  for (auto& pending_msg3 : pending_msg3s) {
    if (not pending_msg3.harq.empty()) {
      pending_msg3.harq.slot_indication(res_alloc.slot_tx());
      if (pending_msg3.harq.has_pending_retx()) {
        schedule_msg3_retx(res_alloc, pending_msg3);
      }
    }
  }
}

void ra_scheduler::run_slot(cell_resource_allocator& res_alloc)
{
  const slot_point pdcch_slot = res_alloc.slot_tx();

  // Handle pending CRCs.
  handle_pending_crc_indications_impl(res_alloc);

  // Pop pending RACHs and process them.
  pending_rachs.slot_indication();
  const span<const rach_indication_message> new_rachs = pending_rachs.get_events();
  for (const rach_indication_message& rach : new_rachs) {
    handle_rach_indication_impl(rach);
  }

  // Ensure slot for RAR PDCCH has DL enabled.
  if (not cell_cfg.is_dl_enabled(pdcch_slot)) {
    // Early exit.
    return;
  }

  // Ensure (i) RA SearchSpace PDCCH monitoring is active for this slot and (ii) there are enough UL symbols to allocate
  // the PDCCH.
  const search_space_id             ss_id  = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id;
  const search_space_configuration& ss_cfg = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[ss_id];
  const coreset_configuration&      cs_cfg = cell_cfg.get_common_coreset(ss_cfg.get_coreset_id());
  // TODO: Handle the case when ra_search_space_id is set to 0.
  if (not is_pdcch_monitoring_active(pdcch_slot, ss_cfg) or
      ss_cfg.get_first_symbol_index() + cs_cfg.duration > cell_cfg.get_nof_dl_symbol_per_slot(pdcch_slot)) {
    // Early exit. RAR scheduling only possible when PDCCH monitoring is active.
    return;
  }

  if (not res_alloc[0].result.dl.csi_rs.empty()) {
    // TODO: Remove this once multiplexing is possible.
    // Early exit. At the moment, we do not multiple PDSCH and CSI-RS.
    return;
  }

  // Ensure there are UL slots where Msg3s can be allocated.
  bool pusch_slots_available = false;
  for (const auto& pusch_td_alloc : get_pusch_time_domain_resource_table(get_pusch_cfg())) {
    const unsigned msg3_delay = get_msg3_delay(pusch_td_alloc, get_ul_bwp_cfg().scs);
    const unsigned start_ul_symbols =
        NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg.get_nof_ul_symbol_per_slot(pdcch_slot + msg3_delay);
    if (cell_cfg.is_ul_enabled(pdcch_slot + msg3_delay) and pusch_td_alloc.symbols.start() >= start_ul_symbols) {
      pusch_slots_available = true;
      break;
    }
  }
  if (not pusch_slots_available) {
    // Early exit. Msg3 scheduling only possible when PUSCH is available.
    return;
  }

  for (auto it = pending_rars.begin(); it != pending_rars.end();) {
    pending_rar_t& rar_req = *it;

    // In case of RAR being outside RAR window:
    // - if window has passed, discard RAR
    // - if window hasn't started, stop loop, as RARs are ordered by slot
    if (not rar_req.rar_window.contains(pdcch_slot)) {
      if (pdcch_slot >= rar_req.rar_window.stop()) {
        fmt::memory_buffer str_buffer;
        fmt::format_to(str_buffer,
                       "Could not transmit RAR within the window={}, prach_slot={}, slot_tx={}",
                       rar_req.rar_window,
                       rar_req.prach_slot_rx,
                       pdcch_slot);
        fmt::print("{}\n", to_c_str(str_buffer));
        logger.warning("{}", to_c_str(str_buffer));
        it = pending_rars.erase(it);
        continue;
      }
      break;
    }

    // Try to schedule DCIs + RBGs for RAR Grants
    const size_t nof_allocs = schedule_rar(rar_req, res_alloc);
    srsran_sanity_check(nof_allocs <= rar_req.tc_rntis.size(), "Invalid number of RAR allocs");

    if (nof_allocs > 0) {
      // If RAR allocation was successful:
      // - in case all Msg3 grants were allocated, remove pending RAR, and continue with following RAR
      // - otherwise, erase only Msg3 grants that were allocated, and stop iteration

      if (nof_allocs == rar_req.tc_rntis.size()) {
        it = pending_rars.erase(it);
      } else {
        // Remove only allocated Msg3 grants
        std::copy(rar_req.tc_rntis.begin() + nof_allocs, rar_req.tc_rntis.end(), rar_req.tc_rntis.begin());
        const size_t new_pending_msg3s =
            rar_req.tc_rntis.size() > nof_allocs ? rar_req.tc_rntis.size() - nof_allocs : 0;
        if (new_pending_msg3s > MAX_PREAMBLES_PER_PRACH_OCCASION) {
          // Note: This check must be added to avoid compilation issue in gcc9.4.0. Potentially a false alarm.
          SRSRAN_UNREACHABLE;
        }
        rar_req.tc_rntis.resize(new_pending_msg3s);
        break;
      }
    } else {
      // If RAR allocation was not successful, try next pending RAR
      ++it;
    }
  }
}

unsigned ra_scheduler::schedule_rar(const pending_rar_t& rar, cell_resource_allocator& res_alloc)
{
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[0];
  if (pdcch_alloc.result.dl.dl_pdcchs.full()) {
    // Early exit.
    log_postponed_rar(rar, "No PDCCH space for RAR.");
    return 0;
  }

  const auto& pdsch_td_res_alloc_list =
      get_ra_rnti_pdsch_time_domain_list(cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common,
                                         cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.cp,
                                         cell_cfg.dmrs_typeA_pos);
  const search_space_configuration& ss_cfg =
      cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common
          .search_spaces[cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id];
  const unsigned coreset_duration = cell_cfg.get_common_coreset(ss_cfg.get_coreset_id()).duration;

  unsigned     pdsch_time_res_index = pdsch_td_res_alloc_list.size();
  unsigned     max_nof_allocs       = 0;
  crb_interval rar_crbs{};
  for (const auto& pdsch_td_res : pdsch_td_res_alloc_list) {
    const unsigned                      time_resource = std::distance(pdsch_td_res_alloc_list.begin(), &pdsch_td_res);
    const cell_slot_resource_allocator& pdsch_alloc   = res_alloc[pdsch_td_res.k0];

    // > Check space in DL sched result for RAR.
    if (pdsch_alloc.result.dl.rar_grants.full()) {
      // early exit.
      log_postponed_rar(rar, "No PDSCH space for RAR.");
      return 0;
    }

    // Ensure slot for RAR PDSCH has DL enabled.
    if (not cell_cfg.is_dl_enabled(pdsch_alloc.slot)) {
      // Early exit.
      return 0;
    }

    // > Check whether PDSCH time domain resource fits in DL symbols of the slot.
    if (pdsch_td_res.symbols.stop() > cell_cfg.get_nof_dl_symbol_per_slot(pdsch_alloc.slot)) {
      continue;
    }

    // > Check whether PDSCH time domain resource does not overlap with CORESET.
    if (pdsch_td_res.symbols.start() < ss_cfg.get_first_symbol_index() + coreset_duration) {
      continue;
    }

    // > Find available RBs in PDSCH for RAR grant.
    const unsigned          nof_rar_rbs = get_nof_pdsch_prbs_required(time_resource, rar.tc_rntis.size()).nof_prbs;
    const ofdm_symbol_range symbols     = pdsch_td_res.symbols;
    const crb_bitmap        used_crbs   = pdsch_alloc.dl_res_grid.used_crbs(get_dl_bwp_cfg().scs, ra_crb_lims, symbols);
    const unsigned          nof_allocs  = rb_helper::find_empty_interval_of_length(used_crbs, nof_rar_rbs, 0).length() /
                                get_nof_pdsch_prbs_required(time_resource, 1).nof_prbs;

    // > Try for maximum allocations as possible.
    if (nof_allocs > max_nof_allocs) {
      max_nof_allocs       = nof_allocs;
      rar_crbs             = rb_helper::find_empty_interval_of_length(used_crbs, nof_rar_rbs, 0);
      pdsch_time_res_index = time_resource;
    }
  }

  if (max_nof_allocs == 0) {
    // Early exit.
    log_postponed_rar(rar, "Not enough PRBs for RAR.");
    return 0;
  }

  if (pdsch_time_res_index == pdsch_td_res_alloc_list.size()) {
    // Early exit.
    log_postponed_rar(rar, "No PDSCH time domain resource found for RAR.");
    return 0;
  }

  // > Find available RBs in PUSCH for Msg3 grants. This process requires searching for a valid K2 value in
  // the list of PUSCH-TimeDomainResourceAllocation in PUSCHConfigCommon.
  static_vector<msg3_alloc_candidate, MAX_GRANTS_PER_RAR> msg3_candidates;
  auto pusch_list = get_pusch_time_domain_resource_table(get_pusch_cfg());
  for (unsigned puschidx = 0; puschidx < pusch_list.size(); ++puschidx) {
    unsigned pusch_res_max_allocs = max_nof_allocs - msg3_candidates.size();
    // >> Verify if Msg3 delay provided by current PUSCH-TimeDomainResourceAllocation corresponds to an UL slot.
    const unsigned                      msg3_delay = get_msg3_delay(pusch_list[puschidx], get_ul_bwp_cfg().scs);
    const cell_slot_resource_allocator& msg3_alloc = res_alloc[msg3_delay];
    const unsigned                      start_ul_symbols =
        NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg.get_nof_ul_symbol_per_slot(msg3_alloc.slot);
    if (not cell_cfg.is_ul_enabled(msg3_alloc.slot) or pusch_list[puschidx].symbols.start() < start_ul_symbols) {
      continue;
    }

    // >> Check space in UL sched result for remaining Msg3s.
    const unsigned list_space = msg3_alloc.result.ul.puschs.capacity() - msg3_alloc.result.ul.puschs.size();
    pusch_res_max_allocs      = std::min(pusch_res_max_allocs, list_space);
    if (pusch_res_max_allocs == 0) {
      continue;
    }

    // >> Check CRBs available in PUSCH for Msg3.
    const unsigned nof_rbs_per_msg3 = msg3_data[puschidx].pusch.rbs.type1().length();
    unsigned       nof_msg3_rbs     = nof_rbs_per_msg3 * pusch_res_max_allocs;
    prb_bitmap     used_ul_crbs     = msg3_alloc.ul_res_grid.used_crbs(get_ul_bwp_cfg(), pusch_list[puschidx].symbols);
    crb_interval   msg3_crbs        = rb_helper::find_empty_interval_of_length(used_ul_crbs, nof_msg3_rbs, 0);
    pusch_res_max_allocs            = msg3_crbs.length() / nof_rbs_per_msg3;
    if (pusch_res_max_allocs == 0) {
      continue;
    }

    // >> Register Msg3 allocations for this PUSCH resource as successful.
    unsigned last_crb = msg3_crbs.start();
    for (unsigned i = 0; i < pusch_res_max_allocs; ++i) {
      msg3_alloc_candidate& candidate = msg3_candidates.emplace_back();
      candidate.crbs                  = {last_crb, last_crb + nof_rbs_per_msg3};
      candidate.pusch_td_res_index    = puschidx;
      last_crb += nof_rbs_per_msg3;
    }
  }
  max_nof_allocs = msg3_candidates.size();
  rar_crbs.resize(get_nof_pdsch_prbs_required(pdsch_time_res_index, max_nof_allocs).nof_prbs);

  // > Find space in PDCCH for RAR.
  const static aggregation_level aggr_lvl = aggregation_level::n4;
  const search_space_id          ss_id    = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id;
  pdcch_dl_information*          pdcch    = pdcch_sch.alloc_pdcch_common(pdcch_alloc, rar.ra_rnti, ss_id, aggr_lvl);
  if (pdcch == nullptr) {
    return 0;
  }

  // Status: RAR allocation is successful.

  // > Fill RAR and Msg3 PDSCH, PUSCH and DCI.
  fill_rar_grant(res_alloc, rar, rar_crbs, pdsch_time_res_index, msg3_candidates);

  return msg3_candidates.size();
}

void ra_scheduler::fill_rar_grant(cell_resource_allocator&         res_alloc,
                                  const pending_rar_t&             rar_request,
                                  crb_interval                     rar_crbs,
                                  unsigned                         pdsch_time_res_index,
                                  span<const msg3_alloc_candidate> msg3_candidates)
{
  const auto& pdsch_td_res_alloc_list =
      get_ra_rnti_pdsch_time_domain_list(cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common,
                                         cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.cp,
                                         cell_cfg.dmrs_typeA_pos);

  cell_slot_resource_allocator& pdcch_alloc = res_alloc[0];
  cell_slot_resource_allocator& rar_alloc   = res_alloc[pdsch_td_res_alloc_list[pdsch_time_res_index].k0];

  // Fill RAR DCI.
  pdcch_dl_information& pdcch = pdcch_alloc.result.dl.dl_pdcchs.back();
  build_dci_f1_0_ra_rnti(
      pdcch.dci, cell_cfg.dl_cfg_common.init_dl_bwp, rar_crbs, pdsch_time_res_index, sched_cfg.rar_mcs_index);

  // Allocate RBs and space for RAR.
  rar_alloc.dl_res_grid.fill(
      grant_info{get_dl_bwp_cfg().scs, pdsch_td_res_alloc_list[pdsch_time_res_index].symbols, rar_crbs});

  // Fill RAR PDSCH.
  rar_information& rar = rar_alloc.result.dl.rar_grants.emplace_back();
  build_pdsch_f1_0_ra_rnti(rar.pdsch_cfg,
                           get_nof_pdsch_prbs_required(pdsch_time_res_index, msg3_candidates.size()).tbs_bytes,
                           pdcch.ctx.rnti,
                           cell_cfg,
                           pdcch.dci.ra_f1_0,
                           rar_crbs,
                           rar_data[pdsch_time_res_index].dmrs_info);

  const auto& pusch_td_alloc_list = get_pusch_time_domain_resource_table(get_pusch_cfg());
  for (unsigned i = 0; i < msg3_candidates.size(); ++i) {
    const auto&                   msg3_candidate = msg3_candidates[i];
    const auto&                   pusch_res      = pusch_td_alloc_list[msg3_candidate.pusch_td_res_index];
    const unsigned                msg3_delay     = get_msg3_delay(pusch_res, get_ul_bwp_cfg().scs);
    cell_slot_resource_allocator& msg3_alloc     = res_alloc[msg3_delay];
    const vrb_interval            vrbs           = msg3_crb_to_vrb(cell_cfg, msg3_candidate.crbs);

    auto& pending_msg3 = pending_msg3s[rar_request.tc_rntis[i] % MAX_NOF_MSG3];
    srsran_sanity_check(pending_msg3.harq.empty(), "Pending Msg3 should not have been added if HARQ is busy.");

    // Allocate Msg3 UL HARQ
    pending_msg3.harq.new_tx(msg3_alloc.slot, sched_cfg.max_nof_msg3_harq_retxs);

    // Add MAC SDU with UL grant (Msg3) in RAR PDU.
    rar_ul_grant& msg3_info            = rar.grants.emplace_back();
    msg3_info.rapid                    = pending_msg3.preamble.preamble_id;
    msg3_info.ta                       = pending_msg3.preamble.time_advance.to_Ta(get_ul_bwp_cfg().scs);
    msg3_info.temp_crnti               = pending_msg3.preamble.tc_rnti;
    msg3_info.time_resource_assignment = msg3_candidate.pusch_td_res_index;
    msg3_info.freq_resource_assignment = ra_frequency_type1_get_riv(ra_frequency_type1_configuration{
        cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(), vrbs.start(), vrbs.length()});
    msg3_info.mcs                      = sched_cfg.msg3_mcs_index;
    // Determine TPC command based on Table 8.2-2, TS 38.213.
    msg3_info.tpc     = (get_pusch_cfg().msg3_delta_power.to_int() + 6) / 2;
    msg3_info.csi_req = false;

    // Allocate Msg3 RBs.
    const ofdm_symbol_range& symbols = pusch_td_alloc_list[msg3_candidate.pusch_td_res_index].symbols;
    msg3_alloc.ul_res_grid.fill(grant_info{get_dl_bwp_cfg().scs, symbols, msg3_candidate.crbs});

    // Fill PUSCH for Msg3.
    ul_sched_info& pusch     = msg3_alloc.result.ul.puschs.emplace_back();
    pusch.context.ue_index   = INVALID_DU_UE_INDEX;
    pusch.context.ss_id      = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id;
    pusch.context.nof_retxs  = 0;
    pusch.context.msg3_delay = msg3_delay;
    pusch.pusch_cfg          = msg3_data[msg3_candidate.pusch_td_res_index].pusch;
    pusch.pusch_cfg.rnti     = pending_msg3.preamble.tc_rnti;
    pusch.pusch_cfg.rbs      = vrbs;
    pusch.pusch_cfg.rv_index = 0;
    pusch.pusch_cfg.new_data = true;

    // Store parameters used in HARQ.
    pending_msg3.harq.save_alloc_params(dci_ul_rnti_config_type::tc_rnti_f0_0, pusch.pusch_cfg);
  }
}

void ra_scheduler::schedule_msg3_retx(cell_resource_allocator& res_alloc, pending_msg3_t& msg3_ctx)
{
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[0];
  const bwp_configuration&      bwp_ul_cmn  = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

  if (not cell_cfg.is_dl_enabled(pdcch_alloc.slot)) {
    // Not possible to schedule Msg3s in this TDD slot.
    return;
  }

  // Verify there is space in PDCCH result lists for new allocations.
  if (pdcch_alloc.result.dl.ul_pdcchs.full()) {
    logger.warning("Failed to allocate PUSCH. Cause: No space available in scheduler output list");
    return;
  }

  const span<const pusch_time_domain_resource_allocation> pusch_td_alloc_list =
      get_pusch_time_domain_resource_table(get_pusch_cfg());
  for (unsigned pusch_td_res_index = 0; pusch_td_res_index != pusch_td_alloc_list.size(); ++pusch_td_res_index) {
    const auto&                   pusch_td_cfg = pusch_td_alloc_list[pusch_td_res_index];
    const unsigned                k2           = pusch_td_cfg.k2;
    cell_slot_resource_allocator& pusch_alloc  = res_alloc[k2];
    const unsigned                start_ul_symbols =
        NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell_cfg.get_nof_ul_symbol_per_slot(pusch_alloc.slot);
    // If it is a retx, we need to ensure we use a time_domain_resource with the same number of symbols as used for
    // the first transmission.
    const bool sym_length_match_prev_grant_for_retx =
        pusch_td_cfg.symbols.length() != msg3_ctx.harq.last_tx_params().nof_symbols;
    if (not cell_cfg.is_ul_enabled(pusch_alloc.slot) or pusch_td_cfg.symbols.start() < start_ul_symbols or
        !sym_length_match_prev_grant_for_retx) {
      // Not possible to schedule Msg3s in this TDD slot.
      continue;
    }

    // Verify there is space in PUSCH and PDCCH result lists for new allocations.
    if (pusch_alloc.result.ul.puschs.full()) {
      logger.warning("Failed to allocate PUSCH. Cause: No space available in scheduler output list");
      continue;
    }

    // Try to reuse previous HARQ PRBs.
    const vrb_interval msg3_vrbs = msg3_ctx.harq.last_tx_params().rbs.type1();
    grant_info         grant;
    grant.scs     = bwp_ul_cmn.scs;
    grant.symbols = pusch_td_cfg.symbols;
    grant.crbs    = msg3_vrb_to_crb(cell_cfg, msg3_vrbs);
    if (pusch_alloc.ul_res_grid.collides(grant)) {
      // Find available symbol x RB resources.
      // TODO
      continue;
    }

    // > Find space in PDCCH for Msg3 DCI.
    // [3GPP TS 38.213, clause 10.1] a UE monitors PDCCH candidates in one or more of the following search spaces sets
    //  - a Type1-PDCCH CSS set configured by ra-SearchSpace in PDCCH-ConfigCommon for a DCI format with
    //    CRC scrambled by a RA-RNTI, a MsgB-RNTI, or a TC-RNTI on the primary cell.
    const search_space_id ss_id = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id;
    pdcch_ul_information* pdcch =
        pdcch_sch.alloc_ul_pdcch_common(pdcch_alloc, msg3_ctx.preamble.tc_rnti, ss_id, aggregation_level::n4);
    if (pdcch == nullptr) {
      logger.debug("tc-rnti={:#x}: Failed to schedule PDCCH for Msg3 retx. Retrying it in a later slot",
                   msg3_ctx.preamble.tc_rnti);
      continue;
    }

    // Check if there are enough UL symbols to allocate the PDCCH
    const search_space_configuration& ss_cfg = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[ss_id];
    const coreset_configuration&      cs_cfg = cell_cfg.get_common_coreset(ss_cfg.get_coreset_id());
    if (ss_cfg.get_first_symbol_index() + cs_cfg.duration > cell_cfg.get_nof_dl_symbol_per_slot(pdcch_alloc.slot)) {
      // Early exit. RAR scheduling only possible when PDCCH monitoring is active.
      return;
    }

    // Mark resources as occupied in the ResourceGrid.
    pusch_alloc.ul_res_grid.fill(grant);

    // Allocate new retx in the HARQ.
    msg3_ctx.harq.new_retx(pusch_alloc.slot);

    // Fill DCI.
    static constexpr uint8_t msg3_rv = 0;
    build_dci_f0_0_tc_rnti(pdcch->dci,
                           cell_cfg.dl_cfg_common.init_dl_bwp,
                           cell_cfg.ul_cfg_common.init_ul_bwp.generic_params,
                           grant.crbs,
                           pusch_td_res_index,
                           msg3_ctx.harq.last_tx_params().mcs,
                           msg3_rv,
                           msg3_ctx.harq);

    // Fill PUSCH.
    ul_sched_info& ul_info     = pusch_alloc.result.ul.puschs.emplace_back();
    ul_info.context.ue_index   = INVALID_DU_UE_INDEX;
    ul_info.context.ss_id      = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id;
    ul_info.context.k2         = k2;
    ul_info.context.nof_retxs  = msg3_ctx.harq.tb().nof_retxs;
    ul_info.pusch_cfg          = msg3_data[pusch_td_res_index].pusch;
    ul_info.pusch_cfg.rnti     = msg3_ctx.preamble.tc_rnti;
    ul_info.pusch_cfg.rbs      = msg3_vrbs;
    ul_info.pusch_cfg.rv_index = pdcch->dci.tc_rnti_f0_0.redundancy_version;
    ul_info.pusch_cfg.new_data = false;

    // Store parameters used in HARQ.
    msg3_ctx.harq.save_alloc_params(dci_ul_rnti_config_type::tc_rnti_f0_0, ul_info.pusch_cfg);

    // successful allocation. Exit loop.
    break;
  }
}

sch_prbs_tbs ra_scheduler::get_nof_pdsch_prbs_required(unsigned time_res_idx, unsigned nof_ul_grants) const
{
  return rar_data[time_res_idx].prbs_tbs_per_nof_grants[std::min(nof_ul_grants, (unsigned)MAX_RAR_PDUS_PER_SLOT) - 1];
}

void ra_scheduler::log_postponed_rar(const pending_rar_t& rar, const char* cause_str) const
{
  logger.debug("RAR allocation for ra-rnti={:#x} was postponed. Cause: {}", rar.ra_rnti, cause_str);
}
