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

#pragma once

#include "../cell/resource_grid.h"
#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "../support/prbs_calculator.h"
#include "../support/slot_event_list.h"
#include "../ue_scheduling/harq_process.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include <deque>

namespace srsran {

class scheduler_event_logger;

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
  /// Implementation-defined limit for maximum number of concurrent Msg3s.
  static constexpr size_t MAX_NOF_MSG3 = 1024;

public:
  explicit ra_scheduler(const scheduler_ra_expert_config& sched_cfg_,
                        const cell_configuration&         cfg_,
                        pdcch_resource_allocator&         pdcch_sched_,
                        scheduler_event_logger&           ev_logger_);

  /// Enqueue RACH indication
  /// \remark See TS 38.321, 5.1.3 - RAP transmission.
  void handle_rach_indication(const rach_indication_message& msg);

  /// Handle UL CRC directed at Msg3 HARQ.
  void handle_crc_indication(const ul_crc_indication& crc_ind);

  /// Allocate pending RARs + Msg3s
  void run_slot(cell_resource_allocator& res_alloc);

private:
  struct pending_rar_t {
    rnti_t                                                  ra_rnti = rnti_t::INVALID_RNTI;
    slot_point                                              prach_slot_rx;
    slot_interval                                           rar_window;
    static_vector<rnti_t, MAX_PREAMBLES_PER_PRACH_OCCASION> tc_rntis;
  };
  struct pending_msg3_t {
    /// Detected PRACH Preamble associated to this Msg3.
    rach_indication_message::preamble preamble{};
    harq_logger msg3_harq_logger{srslog::fetch_basic_logger("SCHED"), rnti_t::INVALID_RNTI, to_du_cell_index(0), false};
    /// UL Harq used to schedule Msg3.
    /// Note: [TS 38.321, 5.4.2.1] "For UL transmission with UL grant in RA Response, HARQ process identifier 0 is
    /// used".
    ul_harq_process harq{to_harq_id(0), msg3_harq_logger};
  };
  struct msg3_alloc_candidate {
    unsigned     pusch_td_res_index;
    crb_interval crbs;
  };

  const bwp_configuration&   get_dl_bwp_cfg() const { return cell_cfg.dl_cfg_common.init_dl_bwp.generic_params; }
  const pdsch_config_common& get_pdsch_cfg() const { return cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common; }
  const bwp_configuration&   get_ul_bwp_cfg() const { return cell_cfg.ul_cfg_common.init_ul_bwp.generic_params; }
  const pusch_config_common& get_pusch_cfg() const { return *cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common; }
  const rach_config_common&  get_rach_cfg() const { return *cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common; }

  /// Pre-compute invariant fields of RAR PDUs (PDSCH, DCI, etc.) for faster scheduling.
  void precompute_rar_fields();

  /// Pre-compute invariant fields of Msg3 PDUs (PUSCH, DCI, etc.) for faster scheduling.
  void precompute_msg3_pdus();

  void handle_rach_indication_impl(const rach_indication_message& msg);

  void handle_pending_crc_indications_impl(cell_resource_allocator& res_alloc);

  void log_postponed_rar(const pending_rar_t& rar, const char* cause_str) const;

  /// Find and allocate DL and UL resources for pending RAR and associated Msg3 grants.
  /// \return The number of allocated Msg3 grants.
  unsigned schedule_rar(const pending_rar_t& rar, cell_resource_allocator& res_alloc);

  /// Schedule RAR grant and associated Msg3 grants in the provided scheduling resources.
  /// \param res_alloc Cell Resource Allocator.
  /// \param rar pending RAR with an associated RA-RNTI that is going to be scheduled.
  /// \param rar_crbs CRBs of the RAR to be scheduled.
  /// \param pdsch_time_res_index Index of PDSCH time domain resource.
  /// \param msg3_candidates List of Msg3s with respective resource information (e.g. RBs and symbols) to allocate.
  void fill_rar_grant(cell_resource_allocator&         res_alloc,
                      const pending_rar_t&             pending_rar,
                      crb_interval                     rar_crbs,
                      unsigned                         pdsch_time_res_index,
                      span<const msg3_alloc_candidate> msg3_candidates);

  /// Schedule retransmission of Msg3.
  void schedule_msg3_retx(cell_resource_allocator& res_alloc, pending_msg3_t& msg3_ctx);

  sch_prbs_tbs get_nof_pdsch_prbs_required(unsigned time_res_idx, unsigned nof_ul_grants) const;

  // args
  const scheduler_ra_expert_config& sched_cfg;
  const cell_configuration&         cell_cfg;
  pdcch_resource_allocator&         pdcch_sch;
  scheduler_event_logger&           ev_logger;

  // derived from args
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

  // variables
  slot_event_list<rach_indication_message> pending_rachs;
  slot_event_list<ul_crc_indication>       pending_crcs;
  std::deque<pending_rar_t>                pending_rars;
  std::vector<pending_msg3_t>              pending_msg3s;
};

} // namespace srsran
