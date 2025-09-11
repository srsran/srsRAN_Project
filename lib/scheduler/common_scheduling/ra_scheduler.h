/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "../cell/cell_harq_manager.h"
#include "../cell/resource_grid.h"
#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "../support/prbs_calculator.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/resource_allocation/rb_bitmap.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/scheduler_feedback_handler.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

class scheduler_event_logger;
class cell_metrics_handler;
struct ul_crc_indication;

/// Get MSG3 Delay.
/// \param[in] pusch_td_res_alloc PUSCH-TimeDomainResourceAllocation.
/// \param[in] pusch_scs SCS used by initial UL BWP.
/// \return Msg3 delay in number of slots.
unsigned get_msg3_delay(const pusch_time_domain_resource_allocation& pusch_td_res_alloc, subcarrier_spacing pusch_scs);

/// \brief Computes the RA-RNTI based on PRACH parameters, as per TS 38.321, Section 5.1.3.
/// \param[in] slot_index Index of the first slot of the PRACH occasion in a system frame. Values {0,...,79}.
/// \param[in] symbol_index Index of the first OFDM symbol of the first PRACH occasion. Values {0,...,13}.
/// \param[in] frequency_index Index of the PRACH occation in the frequency domain. Values {0,...,7}.
/// \param[in] is_sul true is this is SUL carrier, false otherwise.
/// \return the RA-RNRI, as per , as per TS 38.321, Section 5.1.3.
uint16_t get_ra_rnti(unsigned slot_index, unsigned symbol_index, unsigned frequency_index, bool is_sul = false);

/// Scheduler for PRACH occasions, RAR PDSCHs and Msg3 PUSCH grants.
class ra_scheduler
{
public:
  explicit ra_scheduler(const scheduler_ra_expert_config& sched_cfg_,
                        const cell_configuration&         cfg_,
                        pdcch_resource_allocator&         pdcch_sched_,
                        scheduler_event_logger&           ev_logger_,
                        cell_metrics_handler&             metrics_handler_);

  /// Enqueue RACH indication
  /// \remark See TS 38.321, 5.1.3 - RAP transmission.
  void handle_rach_indication(const rach_indication_message& msg);

  /// Handle UL CRC directed at Msg3 HARQ.
  void handle_crc_indication(const ul_crc_indication& crc_ind);

  /// Allocate pending RARs + Msg3s
  void run_slot(cell_resource_allocator& res_alloc);

  void stop();

private:
  class msg3_harq_timeout_notifier;

  struct pending_rar_failed_attempts_t {
    unsigned pdcch = 0;
    unsigned pdsch = 0;
    unsigned pusch = 0;
  };
  struct pending_rar_t {
    rnti_t                                                  ra_rnti = rnti_t::INVALID_RNTI;
    slot_point                                              prach_slot_rx;
    slot_point                                              last_sched_try_slot;
    slot_interval                                           rar_window;
    static_vector<rnti_t, MAX_PREAMBLES_PER_PRACH_OCCASION> tc_rntis;
    pending_rar_failed_attempts_t                           failed_attempts;
  };
  struct pending_msg3_t {
    /// Detected PRACH Preamble associated to this Msg3.
    rach_indication_message::preamble preamble{};
    /// UL Harq used to schedule Msg3.
    /// Note: [TS 38.321, 5.4.2.1] "For UL transmission with UL grant in RA Response, HARQ process identifier 0 is
    /// used".
    unique_ue_harq_entity msg3_harq_ent;

    bool busy() const { return not msg3_harq_ent.empty(); }
  };
  struct msg3_alloc_candidate {
    unsigned     pusch_td_res_index;
    crb_interval crbs;
  };

  using rach_indication_queue = concurrent_queue<rach_indication_message,
                                                 concurrent_queue_policy::lockfree_mpmc,
                                                 concurrent_queue_wait_policy::non_blocking>;
  using crc_indication_queue  = concurrent_queue<ul_crc_indication,
                                                 concurrent_queue_policy::lockfree_mpmc,
                                                 concurrent_queue_wait_policy::non_blocking>;

  const bwp_configuration&   get_dl_bwp_cfg() const { return cell_cfg.dl_cfg_common.init_dl_bwp.generic_params; }
  const pdsch_config_common& get_pdsch_cfg() const { return cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common; }
  const bwp_configuration&   get_ul_bwp_cfg() const { return cell_cfg.ul_cfg_common.init_ul_bwp.generic_params; }
  const pusch_config_common& get_pusch_cfg() const { return *cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common; }
  const rach_config_common&  get_rach_cfg() const { return *cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common; }

  /// Pre-compute invariant fields of RAR PDUs (PDSCH, DCI, etc.) for faster scheduling.
  void precompute_rar_fields();

  /// Pre-compute invariant fields of Msg3 PDUs (PUSCH, DCI, etc.) for faster scheduling.
  void precompute_msg3_pdus();

  void handle_rach_indication_impl(const rach_indication_message& msg, slot_point sl_tx);

  void handle_pending_crc_indications_impl(cell_resource_allocator& res_alloc);

  void
  log_postponed_rar(const pending_rar_t& rar, const char* cause_str, std::optional<slot_point> sl = std::nullopt) const;

  /// Delete RARs that are out of the RAR window.
  void update_pending_rars(slot_point pdcch_slot);

  /// Determines whether the resource grid for the provided slot has the conditions for RAR scheduling.
  bool is_slot_candidate_for_rar(const cell_slot_resource_allocator& slot_res_alloc);

  /// Try scheduling pending RARs for the provided slot.
  void schedule_pending_rars(cell_resource_allocator& res_alloc, slot_point pdcch_slot);

  /// Find and allocate DL and UL resources for pending RAR and associated Msg3 grants.
  /// \return The number of allocated Msg3 grants.
  unsigned schedule_rar(pending_rar_t& rar, cell_resource_allocator& res_alloc, slot_point pdcch_slot);

  /// Schedule RAR grant and associated Msg3 grants in the provided scheduling resources.
  /// \param res_alloc Cell Resource Allocator.
  /// \param pending_rar pending RAR with an associated RA-RNTI that is going to be scheduled.
  /// \param pdcch_slot Slot where the PDCCH is going to be scheduled.
  /// \param rar_crbs CRBs of the RAR to be scheduled.
  /// \param pdsch_time_res_index Index of PDSCH time domain resource.
  /// \param msg3_candidates List of Msg3s with respective resource information (e.g. RBs and symbols) to allocate.
  void fill_rar_grant(cell_resource_allocator&         res_alloc,
                      const pending_rar_t&             pending_rar,
                      slot_point                       pdcch_slot,
                      crb_interval                     rar_crbs,
                      unsigned                         pdsch_time_res_index,
                      span<const msg3_alloc_candidate> msg3_candidates);

  /// Schedule retransmission of Msg3.
  void schedule_msg3_retx(cell_resource_allocator& res_alloc, pending_msg3_t& msg3_ctx) const;

  sch_prbs_tbs get_nof_pdsch_prbs_required(unsigned time_res_idx, unsigned nof_ul_grants) const;

  // Set the max number of slots the scheduler can look ahead in the resource grid (with respect to the current slot) to
  // find PDSCH space for RAR.
  static constexpr unsigned max_dl_slots_ahead_sched = 8U;

  // Args.
  const scheduler_ra_expert_config& sched_cfg;
  const cell_configuration&         cell_cfg;
  pdcch_resource_allocator&         pdcch_sch;
  scheduler_event_logger&           ev_logger;
  cell_metrics_handler&             metrics_hdlr;

  // Derived from args.
  srslog::basic_logger& logger = srslog::fetch_basic_logger("SCHED");
  /// RA window size in number of slots.
  const unsigned ra_win_nof_slots;
  crb_interval   ra_crb_lims;
  const bool     prach_format_is_long;

  /// Pre-cached information related to RAR for a given PDSCH time resource.
  struct rar_param_cached_data {
    dmrs_information dmrs_info;
    /// Number of PRBs and TBS required for different numbers of grants. Index=0 corresponds to 1 grant.
    std::vector<sch_prbs_tbs> prbs_tbs_per_nof_grants;
  };
  std::vector<rar_param_cached_data> rar_data;

  /// Modulation and coding scheme.
  sch_mcs_description rar_mcs_config;

  /// Pre-cached information related to Msg3 for different PUSCH time resources.
  struct msg3_param_cached_data {
    dci_ul_info       dci;
    pusch_information pusch;
  };
  std::vector<msg3_param_cached_data> msg3_data;
  sch_mcs_description                 msg3_mcs_config;

  // Variables.
  cell_harq_manager     msg3_harqs;
  rach_indication_queue pending_rachs;
  crc_indication_queue  pending_crcs;

  /// The maximum number of pending RARs is given by the maximum number of PRACH occasions that can accumulate from a
  /// given UL slot (at which the PRACH is received) until the expiration of the RAR window. The worst case is when:
  /// (i) the PRACH is received instantaneously by the scheduler, and the PRACH slot is the farthest possible from the
  ///     beginning of the start of the RAR window.
  /// (ii) RAR window is min(80 slots, 10 ms).
  /// (iii) there are PRACHs occasions in every UL slot.
  /// (iv) there are no suitable DL slots for scheduling the RARs (pending RARs will be in the vector until the RAR
  ///      window expires).
  /// [Implementation-defined] Assume 80 slots RAR window + TDD 2D1S7D with 30kHz SCS slots and
  /// MAX_PRACH_OCCASIONS_PER_SLOT (the actual number would depend on the PRACH configuration index).
  static constexpr size_t                                                             MAX_PENDING_RARS_SLOTS = 90U;
  static_vector<pending_rar_t, MAX_PRACH_OCCASIONS_PER_SLOT * MAX_PENDING_RARS_SLOTS> pending_rars;
  std::vector<pending_msg3_t>                                                         pending_msg3s;

  // Bitmap of CRBs that might be used for PUCCH transmissions, to avoid scheduling MSG3-PUSCH over them.
  crb_bitmap pucch_crbs;
};

} // namespace srsran
