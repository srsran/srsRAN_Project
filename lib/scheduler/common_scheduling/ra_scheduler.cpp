/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ra_scheduler.h"
#include "../../ran/gnb_format.h"
#include "../pdcch_scheduling/pdcch_config_helpers.h"
#include "../pdcch_scheduling/pdcch_scheduler_impl.h"
#include "../support/config_helpers.h"
#include "../support/dmrs_helpers.h"
#include "srsgnb/ran/resource_allocation/resource_allocation_frequency.h"

using namespace srsgnb;

unsigned srsgnb::get_msg3_delay(const pusch_time_domain_resource_allocation& pusch_td_res_alloc,
                                subcarrier_spacing                           pusch_scs)
{
  // In TS 38.214, Table 6.1.2.1.1-5, Delta is only defined for PUSCH SCS within [kHz15, kHz120kHz].
  srsgnb_sanity_check(to_numerology_value(pusch_scs) <= to_numerology_value(subcarrier_spacing::kHz120),
                      "PUSCH subcarrier spacing not supported for MSG3 delay");

  // The array represents Table 6.1.2.1.1-5, in TS 38.214.
  static const std::array<uint8_t, 4> DELTAS{2, 3, 4, 6};

  // The MSG3 slot is defined as MSG3_slot = floor( n * (2^*(mu_PUSCH) ) / (2^*(mu_PDCCH) ) ) + k2 + Delta.
  // Given the assumption mu_PUSCH == mu_PDCCH, MSG3_delay simplifies to MSG3_delay =  k2 + Delta
  // [TS 38.214, Section 6.1.2.1 and 6.1.2.1.1].
  return static_cast<int>(pusch_td_res_alloc.k2 + DELTAS[to_numerology_value(pusch_scs)]);
}

uint16_t srsgnb::get_ra_rnti(slot_point sl_rx, unsigned symbol_index, unsigned frequency_index, bool is_sul)
{
  // See 38.321, 5.1.3 - Random Access Preamble transmission
  // RA-RNTI = 1 + s_id + 14 × t_id + 14 × 80 × f_id + 14 × 80 × 8 × ul_carrier_id
  // s_id = index of the first OFDM symbol (0 <= s_id < 14)
  // t_id = index of first slot of the PRACH (0 <= t_id < 80)
  // f_id = index of the PRACH in the freq domain (0 <= f_id < 8) (for FDD, f_id=0)
  // ul_carrier_id = 0 for NUL and 1 for SUL carrier
  uint16_t ra_rnti = 1U + symbol_index + 14U * sl_rx.slot_index() + 14U * 80U * frequency_index +
                     (14U * 80U * 8U * (is_sul ? 1U : 0U));
  return ra_rnti;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ra_scheduler::ra_scheduler(const cell_configuration& cfg_, pdcch_scheduler& pdcch_sch_) :
  cfg(cfg_),
  pdcch_sch(pdcch_sch_),
  ra_win_nof_slots(cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.ra_resp_window),
  initial_active_dl_bwp(cfg.dl_cfg_common.init_dl_bwp.generic_params),
  pending_msg3s(MAX_NOF_MSG3)
{
  if (cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value()) {
    // See 38.213 - If a UE is not provided initialDownlinkBWP, an initial active DL BWP is defined by a location
    // and number of contiguous PRBs, starting from a PRB with the lowest index and ending at a PRB with the highest
    // index among PRBs of a CORESET for Type0-PDCCH CSS set, and a SCS and a cyclic prefix for PDCCH reception in the
    // CORESET for Type0-PDCCH CSS set.
    initial_active_dl_bwp.crbs = get_coreset0_crbs(cfg.dl_cfg_common.init_dl_bwp.pdcch_common);
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
    uint16_t ra_rnti = get_ra_rnti(msg.slot_rx, prach_occ.start_symbol, prach_occ.frequency_index);

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
    if (cfg.tdd_cfg_common.has_value()) {
      // TDD case.
      unsigned period = nof_slots_per_tdd_period(*cfg.tdd_cfg_common);
      for (unsigned sl_idx = 0; sl_idx < period; ++sl_idx) {
        slot_point sl_start = rar_req->prach_slot_rx + prach_duration + sl_idx;
        if (cfg.is_dl_enabled(sl_start)) {
          rar_req->rar_window = {sl_start, sl_start + ra_win_nof_slots};
          break;
        }
      }
      srsgnb_sanity_check(rar_req->rar_window.length() != 0, "Invalid configuration");
    } else {
      // FDD case.
      rar_req->rar_window = {rar_req->prach_slot_rx + prach_duration,
                             rar_req->prach_slot_rx + prach_duration + ra_win_nof_slots};
    }

    for (const auto& prach_preamble : prach_occ.preambles) {
      logger.info("SCHED: New PRACH slot={}, preamble={}, ra-rnti=0x{:x}, temp_crnti=0x{:x}, ta_cmd={}",
                  msg.slot_rx,
                  prach_preamble.preamble_id,
                  ra_rnti,
                  prach_preamble.tc_rnti,
                  prach_preamble.time_advance.to_Ta(get_ul_bwp_cfg().scs));

      // Check if TC-RNTI value to be scheduled is already under use
      if (not pending_msg3s[prach_preamble.tc_rnti % MAX_NOF_MSG3].harq.empty()) {
        logger.warning("PRACH ignored, as the allocated TC-RNTI=0x{:x} is already under use", prach_preamble.tc_rnti);
        continue;
      }

      // Store TC-RNTI of the preamble.
      rar_req->tc_rntis.emplace_back(prach_preamble.tc_rnti);

      // Store Msg3 to allocate.
      pending_msg3s[prach_preamble.tc_rnti % MAX_NOF_MSG3].preamble = prach_preamble;
    }
  }
}

void ra_scheduler::handle_crc_indication(const ul_crc_pdu_indication& crc)
{
  pending_crcs.push(crc);
}

void ra_scheduler::handle_pending_crc_indications_impl(cell_resource_allocator& res_alloc)
{
  // Pop pending CRCs and process them.
  pending_crcs.slot_indication();
  span<const ul_crc_pdu_indication> new_crcs = pending_crcs.get_events();
  for (const ul_crc_pdu_indication& crc : new_crcs) {
    auto& pending_msg3 = pending_msg3s[crc.rnti % MAX_NOF_MSG3];
    if (pending_msg3.preamble.tc_rnti != crc.rnti) {
      logger.warning("Invalid UL CRC, cell={}, rnti={:#x}, h_id={}. Cause: Inexistent rnti.",
                     cfg.cell_index,
                     crc.rnti,
                     crc.harq_id);
      continue;
    }
    if (pending_msg3.harq.pid != crc.harq_id) {
      logger.warning("Invalid UL CRC, cell={}, rnti={:#x}, h_id={}. Cause: HARQ-Ids do not match ({} != {})",
                     cfg.cell_index,
                     crc.rnti,
                     crc.harq_id,
                     crc.harq_id,
                     pending_msg3.harq.pid);
      continue;
    }
    // TODO: Fetch TB.
    pending_msg3.harq.ack_info(0, crc.tb_crc_success);
  }

  // Allocate pending Msg3 retransmissions.
  slot_point sl_rx = res_alloc.slot_tx() - 4U;
  for (auto& pending_msg3 : pending_msg3s) {
    if (not pending_msg3.harq.empty()) {
      pending_msg3.harq.slot_indication(sl_rx);
      if (pending_msg3.harq.has_pending_retx()) {
        // TODO: Support Msg3 retransmissions.
        pending_msg3.harq.reset();
      }
    }
  }
}

void ra_scheduler::run_slot(cell_resource_allocator& res_alloc)
{
  slot_point pdcch_slot = res_alloc.slot_tx();

  // Handle pending CRCs.
  handle_pending_crc_indications_impl(res_alloc);

  // Pop pending RACHs and process them.
  pending_rachs.slot_indication();
  span<const rach_indication_message> new_rachs = pending_rachs.get_events();
  for (const rach_indication_message& rach : new_rachs) {
    handle_rach_indication_impl(rach);
  }

  // Ensure slot for RAR PDCCH+PDSCH has DL enabled.
  if (not cfg.is_dl_enabled(pdcch_slot)) {
    // Early exit. RAR scheduling only possible when PDCCH and PDSCH are available.
    return;
  }

  // Ensure RA SearchSpace PDCCH monitoring is active for this slot.
  search_space_id ss_id = cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id;
  if (not is_pdcch_monitoring_active(pdcch_slot, cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[ss_id])) {
    // Early exit. RAR scheduling only possible when PDCCH monitoring is active.
    return;
  }

  // Ensure there are UL slots where Msg3s can be allocated.
  bool pusch_slots_available = false;
  for (const auto& pusch_td_alloc :
       get_pusch_time_domain_resource_table(*cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common)) {
    unsigned msg3_delay = get_msg3_delay(pusch_td_alloc, get_ul_bwp_cfg().scs);
    if (cfg.is_ul_enabled(pdcch_slot + msg3_delay)) {
      pusch_slots_available = true;
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
                       "SCHED: Could not transmit RAR within the window={}, prach_slot={}, slot_tx={}",
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
    size_t nof_allocs = schedule_rar(rar_req, res_alloc);
    srsgnb_sanity_check(nof_allocs <= rar_req.tc_rntis.size(), "Invalid number of RAR allocs");

    if (nof_allocs > 0) {
      // If RAR allocation was successful:
      // - in case all Msg3 grants were allocated, remove pending RAR, and continue with following RAR
      // - otherwise, erase only Msg3 grants that were allocated, and stop iteration

      if (nof_allocs == rar_req.tc_rntis.size()) {
        it = pending_rars.erase(it);
      } else {
        // Remove only allocated Msg3 grants
        std::copy(rar_req.tc_rntis.begin() + nof_allocs, rar_req.tc_rntis.end(), rar_req.tc_rntis.begin());
        size_t new_pending_msg3s = rar_req.tc_rntis.size() > nof_allocs ? rar_req.tc_rntis.size() - nof_allocs : 0;
        rar_req.tc_rntis.resize(new_pending_msg3s);
        break;
      }
    } else {
      // If RAR allocation was not successful, try next pending RAR
      ++it;
    }
  }

  // Log allocated RARs.
  log_rars(res_alloc);
}

unsigned ra_scheduler::schedule_rar(const pending_rar_t& rar, cell_resource_allocator& res_alloc)
{
  // RAR payload size in bytes as per TS38.321, 6.1.5 and 6.2.3.
  //  static const unsigned rar_payload_size_bytes = 7, rar_subheader_size_bytes = 1;
  // TODO: Make smarter algorithm for RAR size derivation.
  static const unsigned nof_prbs_per_rar = 4, nof_prbs_per_msg3 = 3;
  static const unsigned pdsch_time_res_index = 0;

  cell_slot_resource_allocator& rar_alloc = res_alloc[0];

  // 1. Check space in DL sched result for RAR.
  if (rar_alloc.result.dl.rar_grants.full() or rar_alloc.result.dl.dl_pdcchs.full()) {
    // early exit.
    log_postponed_rar(rar, "No PDCCH/PDSCH space for RAR.");
    return 0;
  }

  // Start with the higher number of Msg3 grants to allocate, and keep decrementing based on available space.
  unsigned max_nof_allocs = rar.tc_rntis.size();

  // 2. Find available RBs in PDSCH for RAR grant.
  crb_interval rar_crbs;
  {
    unsigned          nof_rar_rbs = nof_prbs_per_rar * max_nof_allocs;
    ofdm_symbol_range symbols =
        cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[pdsch_time_res_index].symbols;
    crb_bitmap used_crbs = rar_alloc.dl_res_grid.used_crbs(initial_active_dl_bwp, symbols);
    rar_crbs             = find_empty_interval_of_length(used_crbs, nof_rar_rbs, 0);
    max_nof_allocs       = rar_crbs.length() / nof_prbs_per_rar;
    if (max_nof_allocs == 0) {
      // early exit
      log_postponed_rar(rar, "Not enough PRBs for RAR.");
      return 0;
    }
  }

  // 3. Find available RBs in PUSCH for Msg3 grants. This process requires searching for a valid K2 value in
  // the list of PUSCH-TimeDomainResourceAllocation in PUSCHConfigCommon.
  static_vector<msg3_alloc_candidate, MAX_GRANTS> msg3_candidates;
  auto                                            pusch_list = get_pusch_time_domain_resource_table(get_pusch_cfg());
  for (unsigned puschidx = 0; puschidx < pusch_list.size(); ++puschidx) {
    unsigned pusch_res_max_allocs = max_nof_allocs - msg3_candidates.size();
    // 3. Verify if Msg3 delay provided by current PUSCH-TimeDomainResourceAllocation corresponds to an UL slot.
    unsigned                      msg3_delay = get_msg3_delay(pusch_list[puschidx], get_ul_bwp_cfg().scs);
    cell_slot_resource_allocator& msg3_alloc = res_alloc[msg3_delay];
    if (not cfg.is_ul_enabled(msg3_alloc.slot)) {
      continue;
    }

    // 4. Check space in UL sched result for remaining Msg3s.
    unsigned list_space  = msg3_alloc.result.ul.puschs.capacity() - msg3_alloc.result.ul.puschs.size();
    pusch_res_max_allocs = std::min(pusch_res_max_allocs, list_space);
    if (pusch_res_max_allocs == 0) {
      continue;
    }

    // 5. Check CRBs available in PUSCH for Msg3.
    unsigned     nof_msg3_prbs = nof_prbs_per_msg3 * pusch_res_max_allocs;
    prb_bitmap   used_ul_crbs  = msg3_alloc.ul_res_grid.used_crbs(get_ul_bwp_cfg(), pusch_list[puschidx].symbols);
    crb_interval msg3_crbs     = find_empty_interval_of_length(used_ul_crbs, nof_msg3_prbs, 0);
    pusch_res_max_allocs       = msg3_crbs.length() / nof_prbs_per_msg3;
    if (pusch_res_max_allocs == 0) {
      continue;
    }

    // 6. Register Msg3 allocations for this PUSCH resource as successful.
    unsigned last_crb = msg3_crbs.start();
    for (unsigned i = 0; i < pusch_res_max_allocs; ++i) {
      msg3_candidates.emplace_back();
      msg3_candidates.back().crbs               = {last_crb, last_crb + nof_prbs_per_msg3};
      msg3_candidates.back().pusch_td_res_index = puschidx;
      last_crb += nof_prbs_per_msg3;
    }
  }
  max_nof_allocs = msg3_candidates.size();
  rar_crbs.resize(nof_prbs_per_rar * max_nof_allocs);

  // 7. Find space in PDCCH for RAR.
  const static aggregation_level aggr_lvl = aggregation_level::n4;
  search_space_id                ss_id    = cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id;
  pdcch_dl_information*          pdcch    = pdcch_sch.alloc_pdcch_common(rar_alloc, rar.ra_rnti, ss_id, aggr_lvl);
  if (pdcch == nullptr) {
    return 0;
  }

  // Status: RAR allocation is successful.

  // 8. Fill RAR and Msg3 PDSCH, PUSCH and DCI.
  fill_rar_grant(res_alloc, rar, rar_crbs, msg3_candidates);

  return msg3_candidates.size();
}

void ra_scheduler::fill_rar_grant(cell_resource_allocator&         res_alloc,
                                  const pending_rar_t&             rar_request,
                                  crb_interval                     rar_crbs,
                                  span<const msg3_alloc_candidate> msg3_candidates)
{
  static const unsigned max_msg3_retxs       = 4;
  static const unsigned msg3_mcs             = 0;
  static const unsigned pdsch_time_res_index = 0;

  cell_slot_resource_allocator& pdcch_alloc = res_alloc[0];
  cell_slot_resource_allocator& rar_alloc   = res_alloc[get_pdsch_cfg().pdsch_td_alloc_list[pdsch_time_res_index].k0];
  prb_interval                  rar_prbs    = crb_to_prb(initial_active_dl_bwp, rar_crbs);

  // Fill RAR DCI.
  pdcch_dl_information& pdcch        = pdcch_alloc.result.dl.dl_pdcchs.back();
  pdcch.dci.type                     = dci_dl_rnti_config_type::ra_f1_0;
  pdcch.dci.ra_f1_0                  = {};
  dci_1_0_ra_rnti_configuration& dci = pdcch.dci.ra_f1_0;
  dci.N_rb_dl_bwp                    = initial_active_dl_bwp.crbs.length();
  dci.frequency_resource             = ra_frequency_type1_get_riv(
      ra_frequency_type1_configuration{dci.N_rb_dl_bwp, rar_prbs.start(), rar_prbs.length()});
  dci.time_resource            = pdsch_time_res_index;
  dci.vrb_to_prb_mapping       = 0; // TODO.
  dci.modulation_coding_scheme = 0;
  dci.tb_scaling               = 0; // TODO.

  // Allocate RBs and space for RAR.
  rar_alloc.dl_res_grid.fill(
      grant_info{get_dl_bwp_cfg().scs, get_pdsch_cfg().pdsch_td_alloc_list[pdsch_time_res_index].symbols, rar_crbs});

  // Fill RAR PDSCH.
  rar_alloc.result.dl.rar_grants.emplace_back();
  rar_information& rar  = rar_alloc.result.dl.rar_grants.back();
  rar.pdcch_cfg         = &pdcch;
  rar.pdsch_cfg.rnti    = rar.pdcch_cfg->ctx.rnti;
  rar.pdsch_cfg.bwp_cfg = rar.pdcch_cfg->ctx.bwp_cfg;
  rar.pdsch_cfg.prbs    = rar_prbs;
  rar.pdsch_cfg.symbols = get_pdsch_cfg().pdsch_td_alloc_list[pdsch_time_res_index].symbols;
  rar.pdsch_cfg.codewords.emplace_back();
  pdsch_codeword& cw             = rar.pdsch_cfg.codewords.back();
  cw.mcs_table                   = pdsch_mcs_table::qam64;
  cw.mcs_index                   = dci.modulation_coding_scheme;
  cw.rv_index                    = 0;
  sch_mcs_description mcs_config = pdsch_mcs_get_config(cw.mcs_table, cw.mcs_index);
  cw.qam_mod                     = mcs_config.modulation;
  cw.target_code_rate            = mcs_config.target_code_rate;
  cw.tb_size_bytes               = 0; // TODO
  rar.pdsch_cfg.dmrs =
      make_dmrs_info_common(cfg.dl_cfg_common.init_dl_bwp.pdsch_common, dci.time_resource, cfg.pci, cfg.dmrs_typeA_pos);

  for (unsigned i = 0; i < msg3_candidates.size(); ++i) {
    const auto&                   msg3_candidate = msg3_candidates[i];
    const auto&                   pusch_res  = get_pusch_cfg().pusch_td_alloc_list[msg3_candidate.pusch_td_res_index];
    unsigned                      msg3_delay = get_msg3_delay(pusch_res, get_ul_bwp_cfg().scs);
    cell_slot_resource_allocator& msg3_alloc = res_alloc[msg3_delay];
    prb_interval                  prbs       = crb_to_prb(get_ul_bwp_cfg(), msg3_candidate.crbs);

    auto& pending_msg3 = pending_msg3s[rar_request.tc_rntis[i] % MAX_NOF_MSG3];
    srsgnb_sanity_check(pending_msg3.harq.empty(), "Pending Msg3 should not have been added if HARQ is busy.");

    // Add MAC SDU with UL grant (Msg3) in RAR PDU.
    rar.grants.emplace_back();
    rar_ul_grant& msg3_info            = rar.grants.back();
    msg3_info.rapid                    = pending_msg3.preamble.preamble_id;
    msg3_info.ta                       = pending_msg3.preamble.time_advance.to_Ta(get_ul_bwp_cfg().scs);
    msg3_info.temp_crnti               = pending_msg3.preamble.tc_rnti;
    msg3_info.time_resource_assignment = msg3_candidate.pusch_td_res_index;
    msg3_info.freq_resource_assignment = ra_frequency_type1_get_riv(ra_frequency_type1_configuration{
        cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(), prbs.start(), prbs.length()});
    msg3_info.mcs                      = 0;
    msg3_info.tpc                      = 0;
    msg3_info.csi_req                  = false;

    // Allocate Msg3 RBs.
    const ofdm_symbol_range& symbols = get_pusch_cfg().pusch_td_alloc_list[msg3_candidate.pusch_td_res_index].symbols;
    msg3_alloc.ul_res_grid.fill(grant_info{get_dl_bwp_cfg().scs, symbols, msg3_candidate.crbs});
    msg3_alloc.result.ul.puschs.emplace_back();

    // Fill PUSCH for Msg3.
    ul_sched_info& pusch    = msg3_alloc.result.ul.puschs.back();
    pusch.crnti             = pending_msg3.preamble.tc_rnti;
    pusch.pusch_cfg.bwp_cfg = &get_ul_bwp_cfg();
    pusch.pusch_cfg.prbs    = prbs;
    pusch.pusch_cfg.symbols = symbols;
    // TODO

    // Allocate Msg3 UL HARQ
    bool success = pending_msg3.harq.new_tx(msg3_alloc.slot, prbs, msg3_mcs, max_msg3_retxs);
    srsgnb_sanity_check(success, "Unexpected HARQ allocation return");
  }
}

void ra_scheduler::log_postponed_rar(const pending_rar_t& rar, const char* cause_str) const
{
  logger.debug("SCHED: RAR allocation for ra-rnti={:#x} was postponed. Cause: {}", rar.ra_rnti, cause_str);
}

/// Helper to log single RAR grant.
void ra_scheduler::log_rar_helper(fmt::memory_buffer& fmtbuf, const rar_information& rar) const
{
  const char* prefix = "";
  fmt::format_to(fmtbuf, "ra-rnti={:#x}, msg3s=[", rar.pdcch_cfg->ctx.rnti);
  for (const rar_ul_grant& msg3 : rar.grants) {
    fmt::format_to(fmtbuf,
                   "{}{{{:#x}: rapid={}, prbs={}, ta={}}}",
                   prefix,
                   msg3.temp_crnti,
                   msg3.rapid,
                   pending_msg3s[msg3.temp_crnti % MAX_NOF_MSG3].harq.prbs().prbs(),
                   msg3.ta);
    prefix = ", ";
  }
  fmt::format_to(fmtbuf, "]");
}

void ra_scheduler::log_rars(const cell_resource_allocator& res_alloc) const
{
  if (not logger.info.enabled()) {
    return;
  }
  const auto&                 rar_alloc = res_alloc[0];
  span<const rar_information> rars      = rar_alloc.result.dl.rar_grants;
  if (rars.empty()) {
    return;
  }

  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, "SCHED: RAR, cell={} ({} allocated):", cfg.cell_index, rars.size());
  for (const rar_information& rar : rars) {
    fmt::format_to(fmtbuf, "\n- ");
    log_rar_helper(fmtbuf, rar);
  }
  logger.info("{}", to_c_str(fmtbuf));
}
