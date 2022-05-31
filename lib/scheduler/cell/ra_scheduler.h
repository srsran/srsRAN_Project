/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RA_SCHEDULER_H
#define SRSGNB_RA_SCHEDULER_H

#include "../support/slot_event_list.h"
#include "../ue/harq_process.h"
#include "pdcch_scheduler.h"
#include "resource_grid.h"
#include <deque>

namespace srsgnb {

/// Get MSG3 Delay.
/// \param[in] pusch_td_res_alloc PUSCH-TimeDomainResourceAllocation.
/// \param[in] pusch_scs SCS used by initial UL BWP.
/// \return Msg3 delay in number of slots.
unsigned get_msg3_delay(const pusch_time_domain_resource_allocation& pusch_td_res_alloc, subcarrier_spacing pusch_scs);

/// Get RA-RNTI based on PRACH parameters.
/// \remark See 38.321, 5.1.3 - Random Access Preamble transmission
uint16_t get_ra_rnti(const rach_indication_message& rach_ind, bool is_sul = false);

/// Scheduler for RAR and Msg3
class ra_scheduler
{
  /// Implementation-defined limit for maximum Msg3s to be allocated in a given RAR.
  static constexpr size_t MAX_MSG3_LIST = 16;
  /// Implementation-defined limit for maximum number of concurrent Msg3s.
  static constexpr size_t MAX_NOF_MSG3 = 1024;

public:
  explicit ra_scheduler(const cell_configuration& cfg_, pdcch_scheduler& pdcch_sched_);

  /// Enqueue RACH indication
  /// \remark See TS 38.321, 5.1.3 - RAP transmission.
  void handle_rach_indication(const rach_indication_message& msg);

  /// Allocate pending RARs + Msg3s
  void run_slot(cell_resource_allocator& res_alloc);

private:
  struct pending_rar_t {
    rnti_t                               ra_rnti = INVALID_RNTI;
    slot_point                           prach_slot_rx;
    slot_interval                        rar_window;
    static_vector<rnti_t, MAX_MSG3_LIST> tc_rntis;
  };
  struct pending_msg3 {
    rach_indication_message ind_msg{};
    /// UL Harq used to schedule Msg3.
    ul_harq_process harq{0};
  };
  struct msg3_alloc_candidate {
    unsigned     pusch_td_res_index;
    crb_interval crbs;
  };

  const bwp_configuration&   get_dl_bwp_cfg() const { return cfg.dl_cfg_common.init_dl_bwp.generic_params; }
  const bwp_configuration&   get_ul_bwp_cfg() const { return cfg.ul_cfg_common.init_ul_bwp.generic_params; }
  const pusch_config_common& get_pusch_cfg() const { return *cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common; }

  bool handle_rach_indication_impl(const rach_indication_message& msg);

  void log_postponed_rar(const pending_rar_t& rar, const char* cause_str) const;
  void log_rars(const cell_resource_allocator& res_alloc) const;
  void log_rar_helper(fmt::memory_buffer& fmtbuf, const rar_information& rar) const;

  /// Find and allocate DL and UL resources for pending RAR and associated Msg3 grants.
  /// \return The number of allocated Msg3 grants.
  unsigned schedule_rar(const pending_rar_t& rar, cell_resource_allocator& res_alloc);

  /// Schedule RAR grant and associated Msg3 grants in the provided scheduling resources.
  /// \param res_alloc Cell Resource Allocator.
  /// \param rar pending RAR with an associated RA-RNTI that is going to be scheduled.
  /// \param rar_crbs CRBs of the RAR to be scheduled.
  /// \param msg3_candidates List of Msg3s with respective resource information (e.g. RBs and symbols) to allocate.
  void fill_rar_grant(cell_resource_allocator&         res_alloc,
                      const pending_rar_t&             pending_rar,
                      crb_interval                     rar_crbs,
                      span<const msg3_alloc_candidate> msg3_candidates);

  // args
  const cell_configuration& cfg;
  pdcch_scheduler&          pdcch_sch;

  // derived from args
  srslog::basic_logger& logger = srslog::fetch_basic_logger("MAC");
  const unsigned        ra_win_nof_slots;

  // variables
  slot_event_list<rach_indication_message> pending_rachs;
  std::deque<pending_rar_t>                pending_rars;
  std::vector<pending_msg3>                pending_msg3s;
};

} // namespace srsgnb

#endif // SRSGNB_RA_SCHEDULER_H
