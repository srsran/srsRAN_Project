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

#ifndef SRSGNB_RACH_HANDLER_H
#define SRSGNB_RACH_HANDLER_H

#include "du_rnti_table.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/scheduler/scheduler_configurator.h"

namespace srsgnb {

/// \brief Handles the allocation of RNTIs and forwarding of RACH indications to scheduler.
/// This class is thread-safe.
class rach_handler final : public mac_rach_handler
{
public:
  explicit rach_handler(scheduler_configurator& sched_, du_rnti_table& rnti_table_) :
    logger(srslog::fetch_basic_logger("MAC")), rnti_table(rnti_table_), sched(sched_)
  {}

  /// Handles detected PRACHs by allocating a temporary C-RNTI and signalling the scheduler to allocate an RAR.
  void handle_rach_indication(const mac_rach_indication& rach_ind) override
  {
    rnti_t alloc_temp_crnti = allocate_rnti();
    if (alloc_temp_crnti != INITIAL_RNTI) {
      notify_sched(rach_ind, alloc_temp_crnti);
    }
  }

  rnti_t allocate_rnti()
  {
    if (rnti_table.nof_ues() >= MAX_NOF_DU_UES) {
      // If the number of UEs is already maximum, ignore RACH.
      return INITIAL_RNTI;
    }
    // Increments rnti counter until it finds an available temp C-RNTI.
    rnti_t temp_crnti;
    do {
      uint16_t prev_counter = rnti_counter.fetch_add(1, std::memory_order_relaxed) % CRNTI_RANGE;
      temp_crnti            = to_rnti(prev_counter + rnti_t::MIN_CRNTI);
    } while (rnti_table.has_rnti(temp_crnti));
    return temp_crnti;
  }

private:
  static constexpr int    CRNTI_RANGE  = rnti_t::MAX_CRNTI + 1 - rnti_t::MIN_CRNTI;
  static constexpr rnti_t INITIAL_RNTI = to_rnti(0x4601);

  /// Notify Scheduler of handled RACH indication.
  void notify_sched(const mac_rach_indication& rach_ind, rnti_t rnti)
  {
    rach_indication_message sched_rach{};
    sched_rach.cell_index      = rach_ind.cell_index;
    sched_rach.timing_info     = 0; // TODO
    sched_rach.slot_rx         = rach_ind.slot_rx;
    sched_rach.symbol_index    = rach_ind.symbol_index;
    sched_rach.frequency_index = rach_ind.frequency_index;
    sched_rach.preamble_id     = rach_ind.preamble_id;
    sched_rach.timing_advance  = rach_ind.timing_advance;
    sched_rach.crnti           = rnti;
    sched.handle_rach_indication(sched_rach);
  }

  srslog::basic_logger& logger;
  du_rnti_table&        rnti_table;
  scheduler_configurator&   sched;

  std::atomic<std::underlying_type_t<rnti_t> > rnti_counter{INITIAL_RNTI - MIN_CRNTI};
};

} // namespace srsgnb

#endif // SRSGNB_RACH_HANDLER_H
