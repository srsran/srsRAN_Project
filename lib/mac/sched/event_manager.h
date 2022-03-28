
#ifndef SRSGNB_CELL_EVENT_MANAGER_H
#define SRSGNB_CELL_EVENT_MANAGER_H

#include "../../ran/gnb_format.h"
#include "cell/cell_sched.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/adt/unique_function.h"
#include "srsgnb/mac/sched_interface.h"
#include "srsgnb/mac/sched_ue_feedback.h"
#include "ue/ue.h"
#include <mutex>

namespace srsgnb {

class event_logger;

class event_manager
{
public:
  event_manager(ue_map_t& ue_db_, std::vector<std::unique_ptr<cell_sched> >& cell_sched_) :
    logger(srslog::fetch_basic_logger("MAC")), ue_db(ue_db_), cell_scheds(cell_sched_)
  {}

  void handle(const cell_configuration_request_message& msg);
  void handle(const sr_indication_message& sr_ind);
  void handle(const ul_bsr_indication_message& bsr_ind);
  void handle(const rach_indication_message& rach_ind);

  void run(slot_point sl_tx, du_cell_index_t cell_index);

private:
  struct event_t {
    rnti_t                               rnti = INVALID_RNTI;
    unique_function<void(event_logger&)> callback;

    template <typename Callable>
    event_t(rnti_t rnti_, Callable&& c) : rnti(rnti_), callback(std::forward<Callable>(c))
    {}
  };

  struct event_list {
    std::mutex           mutex;
    std::vector<event_t> next_events, current_events;
  };

  void process_common(slot_point sl_tx);
  bool cell_exists(du_cell_index_t cell_index) const;
  bool event_requires_sync(const event_t& ev, bool verbose) const;
  void log_invalid_rnti(const event_t& ev) const;
  void log_invalid_cc(const event_t& ev) const;

  srslog::basic_logger&                      logger;
  ue_map_t&                                  ue_db;
  std::vector<std::unique_ptr<cell_sched> >& cell_scheds;

  /// Pending Events list per cell.
  std::vector<std::unique_ptr<event_list> > cell_events;

  /// Pending Events list common to all cells. We use this list for events that require synchronization across
  /// UE carriers when CA is enabled (e.g. SR, BSR, reconfig).
  event_list common_events;
  slot_point last_sl_tx;
};

} // namespace srsgnb

#endif // SRSGNB_CELL_EVENT_MANAGER_H
