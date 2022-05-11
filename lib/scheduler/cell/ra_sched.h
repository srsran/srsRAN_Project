/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RA_SCHED_H
#define SRSGNB_RA_SCHED_H

#include "../sched_harq.h"
#include "resource_grid.h"
#include <deque>

namespace srsgnb {

/// Scheduler for RAR and Msg3
class ra_sched
{
  // Implementation-defined limit for maximum Msg3s to be allocated in a given RAR.
  static constexpr size_t MAX_MSG3_LIST = 16;
  /// Implementation-defined limit for maximum number of concurrent Msg3s.
  static constexpr size_t MAX_NOF_MSG3 = 1024;

public:
  explicit ra_sched(const cell_configuration& cfg_);

  /// Enqueue RACH indication
  /// See TS 38.321, 5.1.3 - RAP transmission
  bool handle_rach_indication(const rach_indication_message& msg);

  /// Allocate pending RARs + Msg3s
  void run_slot(cell_resource_grid& res_alloc);

private:
  struct pending_rar_t {
    rnti_t                               ra_rnti = INVALID_RNTI;
    slot_point                           prach_slot_rx;
    slot_interval                        rar_window;
    static_vector<rnti_t, MAX_MSG3_LIST> tc_rntis;
  };
  struct pending_msg3_t {
    rach_indication_message ind_msg{};
    ul_harq_proc            msg3_harq{0};
  };

  void log_postponed_rar(const pending_rar_t& rar, const char* cause_str);
  void log_rar(const rar_information& rar);

  /// Allocate pending RAR and associated Msg3 grants
  /// \return The number of allocated Msg3 grants
  unsigned
  allocate_rar(const pending_rar_t& rar, cell_slot_resource_grid& rar_alloc, cell_slot_resource_grid& msg3_alloc);

  void fill_rar_grant(const pending_rar_t&     rar,
                      const prb_interval&      rar_prbs,
                      const prb_interval&      msg3_prbs,
                      cell_slot_resource_grid& rar_alloc,
                      cell_slot_resource_grid& msg3_alloc,
                      unsigned                 nof_msg3_grants);

  // args
  const cell_configuration& cfg;

  // derived from args
  srslog::basic_logger& logger = srslog::fetch_basic_logger("MAC");
  const unsigned        ra_win_nof_slots;

  // variables
  std::deque<pending_rar_t>   pending_rars;
  std::vector<pending_msg3_t> pending_msg3s;
};

} // namespace srsgnb

#endif // SRSGNB_RA_SCHED_H
