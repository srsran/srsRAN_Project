
#ifndef SRSGNB_CELL_EVENT_MANAGER_H
#define SRSGNB_CELL_EVENT_MANAGER_H

#include "../../ran/gnb_format.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/adt/unique_function.h"
#include "srsgnb/mac/sched_interface.h"
#include "srsgnb/mac/sched_ue_feedback.h"
#include "ue/ue.h"
#include <mutex>

namespace srsgnb {

class event_logger;
class cell_sched_manager;

/// \brief Class used to manage events that arrive to the scheduler. It acts as a facade for the several subcomponents
/// of the scheduler.
/// The event_manager tries to ensure no race conditions occur while applying the operations that derive from an event.
/// Depending on the type of event, the event_manager may decide to enqueue the event for async processing or
/// process right at the callee in a synchronous fashion.
class event_manager
{
public:
  event_manager(ue_map_t& ue_db_, cell_sched_manager& cell_sched_) :
    logger(srslog::fetch_basic_logger("MAC")), ue_db(ue_db_), cells(cell_sched_)
  {}

  /// Enqueue scheduler events.
  void handle_cell_configuration_request(const cell_configuration_request_message& msg);
  void handle_sr_indication(const sr_indication_message& sr_ind);
  void handle_ul_bsr(const ul_bsr_indication_message& bsr_ind);
  void handle_rach_indication(const rach_indication_message& rach_ind);

  /// Process events for a given slot and cell index.
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
    std::mutex mutex;
    /// Stores all enqueued that are going to be processed in the next slot_indication, ie slot_tx + 1.
    std::vector<event_t> next_events;
    /// Contains the events being processed in the current slot, ie slot_tx.
    /// Note: the transfer of next_events to current_events is done via a std::swap, which for std::vector is very fast.
    std::vector<event_t> current_events;
  };

  void process_common(slot_point sl_tx);
  bool cell_exists(du_cell_index_t cell_index) const;

  /// Checks whether the event requires synchronization across cells. Examples include activating component carriers
  /// in case of CA, or events directed at UEs with CA enabled.
  bool event_requires_sync(const event_t& ev, bool verbose) const;
  void log_invalid_rnti(const event_t& ev) const;
  void log_invalid_cc(const event_t& ev) const;

  srslog::basic_logger& logger;
  ue_map_t&             ue_db;
  cell_sched_manager&   cells;

  /// Pending Events list per cell.
  std::array<std::unique_ptr<event_list>, MAX_NOF_CELLS> events_per_cell_list;

  /// Pending Events list common to all cells. We use this list for events that require synchronization across
  /// UE carriers when CA is enabled (e.g. SR, BSR, reconfig).
  event_list common_events;
  slot_point last_sl_tx;
};

} // namespace srsgnb

#endif // SRSGNB_CELL_EVENT_MANAGER_H
