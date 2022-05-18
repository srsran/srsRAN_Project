/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ra_sched.h"
#include "../../ran/gnb_format.h"

using namespace srsgnb;

unsigned srsgnb::get_msg3_delay(const pusch_time_domain_resource_allocation& pusch_td_res_alloc,
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

uint16_t srsgnb::get_ra_rnti(const rach_indication_message& rach_ind, bool is_sul)
{
  // See 38.321, 5.1.3 - Random Access Preamble transmission
  // RA-RNTI = 1 + s_id + 14 × t_id + 14 × 80 × f_id + 14 × 80 × 8 × ul_carrier_id
  // s_id = index of the first OFDM symbol (0 <= s_id < 14)
  // t_id = index of first slot of the PRACH (0 <= t_id < 80)
  // f_id = index of the PRACH in the freq domain (0 <= f_id < 8) (for FDD, f_id=0)
  // ul_carrier_id = 0 for NUL and 1 for SUL carrier
  uint16_t ra_rnti = 1 + rach_ind.symbol_index + 14 * rach_ind.slot_rx.slot_index() +
                     14 * 80 * rach_ind.frequency_index + (14 * 80 * 8 * (is_sul ? 1 : 0));
  return ra_rnti;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ra_sched::ra_sched(const cell_configuration& cfg_) :
  cfg(cfg_),
  ra_win_nof_slots(cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.ra_resp_window),
  pending_msg3s(MAX_NOF_MSG3)
{}

bool ra_sched::handle_rach_indication(const rach_indication_message& msg)
{
  const static unsigned prach_duration = 1; // TODO: Take from config

  uint16_t ra_rnti = get_ra_rnti(msg);

  logger.info("SCHED: New PRACH slot={}, preamble={}, ra-rnti=0x{:x}, temp_crnti=0x{:x}, ta_cmd={}",
              msg.slot_rx,
              msg.preamble_id,
              ra_rnti,
              msg.crnti,
              msg.timing_advance);

  // Check if TC-RNTI value to be scheduled is already under use
  if (not pending_msg3s[msg.crnti % MAX_NOF_MSG3].msg3_harq.empty()) {
    logger.warning("PRACH ignored, as the allocated TC-RNTI=0x{:x} is already under use", msg.crnti);
    return false;
  }

  // Find pending RAR with same RA-RNTI
  bool rar_found = false;
  for (pending_rar_t& r : pending_rars) {
    if (r.prach_slot_rx == msg.slot_rx and ra_rnti == r.ra_rnti) {
      if (r.tc_rntis.full()) {
        logger.warning("PRACH ignored, as the the maximum number of RAR grants per slot has been reached.");
        return false;
      }
      r.tc_rntis.push_back(msg.crnti);
      rar_found = true;
      break;
    }
  }

  if (not rar_found) {
    // Create new pending RAR
    pending_rars.emplace_back();
    auto& rar_req         = pending_rars.back();
    rar_req.ra_rnti       = to_rnti(ra_rnti);
    rar_req.prach_slot_rx = msg.slot_rx;
    // First slot after PRACH with active DL slot represents the start of the RAR window.
    if (cfg.tdd_cfg_common.has_value()) {
      // TDD case.
      unsigned period = nof_slots_per_tdd_period(*cfg.tdd_cfg_common);
      for (unsigned sl_idx = 0; sl_idx < period; ++sl_idx) {
        slot_point sl_start = rar_req.prach_slot_rx + prach_duration + sl_idx;
        if (cfg.is_dl_enabled(sl_start)) {
          rar_req.rar_window = {sl_start, sl_start + ra_win_nof_slots};
          break;
        }
      }
      srsran_sanity_check(rar_req.rar_window.length() != 0, "Invalid configuration");
    } else {
      // FDD case.
      rar_req.rar_window = {rar_req.prach_slot_rx + prach_duration,
                            rar_req.prach_slot_rx + prach_duration + ra_win_nof_slots};
    }
    rar_req.tc_rntis.push_back(msg.crnti);
  }

  // Store Msg3 to allocate
  pending_msg3s[msg.crnti % MAX_NOF_MSG3].ind_msg = msg;

  return true;
}

void ra_sched::run_slot(cell_resource_allocator& res_alloc)
{
  slot_point pdcch_slot = res_alloc.slot_tx();

  // Ensure slot for RAR PDCCH+PDSCH has DL enabled.
  if (not cfg.is_dl_enabled(pdcch_slot)) {
    // Early exit. RAR scheduling only possible when PDCCH and PDSCH are available.
    return;
  }

  // Find slots where Msg3s can be allocated in PUSCH.
  bool pusch_slots_available = false;
  for (const auto& pusch_td_alloc : cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list) {
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
      return;
    }

    // Try to schedule DCIs + RBGs for RAR Grants
    size_t nof_allocs = allocate_rar(rar_req, res_alloc);
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
        size_t new_pending_msg3s = rar_req.tc_rntis.size() > nof_allocs ? rar_req.tc_rntis.size() - nof_allocs : 0;
        rar_req.tc_rntis.resize(new_pending_msg3s);
        break;
      }
    } else {
      // If RAR allocation was not successful, try next pending RAR
      ++it;
    }
  }
}

unsigned ra_sched::allocate_rar(const pending_rar_t& rar, cell_resource_allocator& res_alloc)
{
  // TODO: Make smarter algorithm for RAR size derivation
  static const unsigned nof_prbs_per_rar = 4, nof_prbs_per_msg3 = 3;

  cell_slot_resource_allocator& rar_alloc = res_alloc[0];

  // 1. Check space in DL sched result for RAR
  if (rar_alloc.result.dl.rar_grants.full()) {
    // early exit
    log_postponed_rar(rar, "No PDSCH space for RAR");
    return 0;
  }

  // Start with the higher number of Msg3 grants to allocate, and keep decrementing based on available space
  unsigned max_nof_allocs = rar.tc_rntis.size();

  // 2. Check PRBs available in PDSCH for RAR.
  crb_interval avail_dl_crbs;
  {
    unsigned          nof_rar_rbs = nof_prbs_per_rar * max_nof_allocs;
    const prb_bitmap& used_crbs   = rar_alloc.dl_res_grid.sch_crbs(get_dl_bwp_cfg());
    avail_dl_crbs                 = find_empty_interval_of_length(used_crbs, nof_rar_rbs, 0);
    max_nof_allocs                = avail_dl_crbs.length() / nof_prbs_per_rar;
    if (max_nof_allocs == 0) {
      // early exit
      log_postponed_rar(rar, "Not enough PRBs for RAR.");
      return 0;
    }
  }

  // 3. Choose a valid PUSCH-TimeDomainResourceAllocation for Msg3.
  // Note: Currently, we stop the search when at least one valid PUSCH-TimeDomainResourceAllocation candidate is found.
  crb_interval                  avail_ul_crbs;
  cell_slot_resource_allocator* msg3_alloc = nullptr;
  for (const auto& pusch_elem : cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list) {
    unsigned                      max_nof_allocs_temp = max_nof_allocs;
    unsigned                      msg3_delay          = get_msg3_delay(pusch_elem, get_ul_bwp_cfg().scs);
    cell_slot_resource_allocator& msg3_alloc_temp     = res_alloc[msg3_delay];
    if (not cfg.is_ul_enabled(msg3_alloc_temp.slot)) {
      continue;
    }

    // 3.1. Check space in UL sched result for Msg3.
    unsigned list_space = msg3_alloc_temp.result.ul.puschs.capacity() - msg3_alloc_temp.result.ul.puschs.size();
    max_nof_allocs_temp = std::min(max_nof_allocs_temp, list_space);
    if (max_nof_allocs_temp == 0) {
      continue;
    }

    // 3.2. Check CRBs available in PUSCH for Msg3.
    unsigned   nof_msg3_prbs = nof_prbs_per_msg3 * max_nof_allocs_temp;
    prb_bitmap used_ul_crbs  = msg3_alloc_temp.ul_res_grid.sch_crbs(get_ul_bwp_cfg());
    avail_ul_crbs            = find_empty_interval_of_length(used_ul_crbs, nof_msg3_prbs, 0);
    max_nof_allocs_temp      = avail_ul_crbs.length() / nof_prbs_per_msg3;
    if (max_nof_allocs_temp == 0) {
      continue;
    }

    // Found space in PUSCH for Msg3.
    max_nof_allocs = max_nof_allocs_temp;
    msg3_alloc     = &msg3_alloc_temp;
    break;
  }
  if (msg3_alloc == nullptr) {
    // early exit.
    log_postponed_rar(rar, "No space in PUSCH for Msg3");
    return 0;
  }

  // 6. Find space in PDCCH for RAR
  // TODO

  // Status: RAR allocation was successful.
  avail_ul_crbs.resize_to(nof_prbs_per_msg3 * max_nof_allocs);
  avail_dl_crbs.resize_to(nof_prbs_per_rar * max_nof_allocs);

  // 7. Fill RAR and Msg3 PDSCH, PUSCH and DCI
  fill_rar_grant(rar, avail_dl_crbs, avail_ul_crbs, rar_alloc, *msg3_alloc, max_nof_allocs);

  return max_nof_allocs;
}

void ra_sched::fill_rar_grant(const pending_rar_t&          rar_request,
                              const crb_interval&           rar_crbs,
                              const crb_interval&           msg3_crbs,
                              cell_slot_resource_allocator& rar_alloc,
                              cell_slot_resource_allocator& msg3_alloc,
                              unsigned                      nof_msg3_grants)
{
  static const unsigned    nof_prbs_per_msg3 = 3;
  static const unsigned    max_msg3_retxs    = 4;
  static const unsigned    msg3_mcs          = 0;
  const ofdm_symbol_range& ofdm_symbols =
      cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].symbols;

  // Allocate PRBs and space for RAR
  // FIXME: Take PDSCH symbols from Config.
  rar_alloc.dl_res_grid.fill(grant_info{grant_info::channel::sch, get_dl_bwp_cfg().scs, ofdm_symbols, rar_crbs});
  rar_alloc.result.dl.rar_grants.emplace_back();
  rar_information& rar = rar_alloc.result.dl.rar_grants.back();

  // Fill RAR DCI
  // TODO

  // Fill RAR information
  rar.cell_index = cfg.cell_index;
  rar.ra_rnti    = rar_request.ra_rnti;

  unsigned last_msg3_start = msg3_crbs.start();
  for (unsigned i = 0; i < nof_msg3_grants; ++i) {
    auto& msg3_req  = pending_msg3s[rar_request.tc_rntis[i] % MAX_NOF_MSG3];
    auto& msg3_harq = msg3_req.msg3_harq;
    srsran_sanity_check(msg3_harq.empty(), "Pending Msg3 should not have been added if HARQ is busy.");

    // Add Msg3 grant in RAR info
    rar.grants.emplace_back();
    msg3_information& msg3_grant = rar.grants.back();
    msg3_grant.rapid             = msg3_req.ind_msg.preamble_id;
    msg3_grant.ta                = msg3_req.ind_msg.timing_advance;
    msg3_grant.temp_crnti        = msg3_req.ind_msg.crnti;

    // Determine Msg3 PRB interval
    msg3_grant.prbs = {last_msg3_start, last_msg3_start + nof_prbs_per_msg3};
    last_msg3_start = last_msg3_start + nof_prbs_per_msg3;

    // Fill Msg3 DCI in RAR
    dci_ul_t msg3_dci{};
    // TODO

    // Allocate and fill PUSCH for Msg3
    // FIXME: Take PUSCH symbols from Config.
    msg3_alloc.ul_res_grid.fill(
        bwp_sch_grant_info(cfg.dl_cfg_common.init_dl_bwp.generic_params, {2, 14}, msg3_grant.prbs));
    msg3_alloc.result.ul.puschs.emplace_back();
    ul_sched_info& pusch = msg3_alloc.result.ul.puschs.back();
    pusch.crnti          = msg3_req.ind_msg.crnti;
    // TODO

    // Allocate Msg3 UL HARQ
    bool success = msg3_harq.new_tx(msg3_alloc.slot, msg3_grant.prbs, msg3_mcs, max_msg3_retxs, msg3_dci);
    srsran_sanity_check(success, "Unexpected HARQ allocation return");
  }

  log_rar(rar);
}

void ra_sched::log_postponed_rar(const pending_rar_t& rar, const char* cause_str)
{
  logger.debug("SCHED: RAR allocation for ra-rnti={} was postponed. Cause: {}", rar.ra_rnti, cause_str);
}

void ra_sched::log_rar(const rar_information& rar)
{
  if (not logger.info.enabled()) {
    return;
  }
  fmt::memory_buffer fmtbuf;
  const char*        prefix = "";
  for (const msg3_information& msg3 : rar.grants) {
    fmt::format_to(
        fmtbuf, "{}{{0x{:x}: rapid={}, prbs={}, ta={}}}", prefix, msg3.temp_crnti, msg3.rapid, msg3.prbs, msg3.ta);
    prefix = ", ";
  }
  logger.info("SCHED: RAR ra-rnti=0x{:x}, cell={}, msg3s=[{}]", rar.ra_rnti, rar.cell_index, to_c_str(fmtbuf));
}
