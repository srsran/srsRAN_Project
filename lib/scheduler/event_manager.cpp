/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "event_manager.h"
#include "cell/cell_sched.h"

using namespace srsgnb;

/// Class used to enqueue all processed events in a slot and log them in a single batch.
class srsgnb::event_logger
{
public:
  event_logger(du_cell_index_t cell_index_, srslog::basic_logger& logger_) :
    cell_index(cell_index_), enabled(logger_.info.enabled()), logger(logger_)
  {}
  event_logger(const event_logger&) = delete;
  event_logger(event_logger&&)      = delete;
  event_logger& operator=(const event_logger&) = delete;
  event_logger& operator=(event_logger&&) = delete;
  ~event_logger()
  {
    if (enabled and fmtbuf.size() > 0) {
      if (cell_index < MAX_NOF_DU_CELLS) {
        logger.info("SCHED: Processed events, cell_index={}: [{}]", cell_index, to_c_str(fmtbuf));
      } else {
        logger.info("SCHED: Processed events: [{}]", cell_index, to_c_str(fmtbuf));
      }
    }
  }

  template <typename... Args>
  void enqueue(const char* fmt, Args&&... args)
  {
    if (enabled) {
      if (fmtbuf.size() > 0) {
        fmt::format_to(fmtbuf, ", ");
      }
      fmt::format_to(fmtbuf, fmt, std::forward<Args>(args)...);
    }
  }

private:
  du_cell_index_t       cell_index;
  bool                  enabled;
  srslog::basic_logger& logger;
  fmt::memory_buffer    fmtbuf;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

event_manager::event_manager(ue_map_t&                     ue_db_,
                             cell_sched_manager&           cell_sched_,
                             sched_configuration_notifier& mac_notifier_) :
  logger(srslog::fetch_basic_logger("MAC")), ue_db(ue_db_), cells(cell_sched_), mac_notifier(mac_notifier_)
{
  (void)mac_notifier;
}

void event_manager::handle_cell_configuration_request(const sched_cell_configuration_request_message& msg)
{
  srsran_assert(not cell_exists(msg.cell_index), "Invalid cell index");
  srsran_sanity_check(is_cell_configuration_request_valid(msg), "Invalid cell configuration");

  cells.add_cell(msg.cell_index, msg);
  events_per_cell_list[msg.cell_index] = std::make_unique<event_list>();
  logger.info("SCHED: Cell with index={} was configured.", msg.cell_index);
}

void event_manager::handle_sr_indication(const sr_indication_message& sr_ind)
{
  srsran_sanity_check(cell_exists(sr_ind.cell_index), "Invalid cell index");

  std::lock_guard<std::mutex> lock(common_events.mutex);
  common_events.next_events.emplace_back(
      sr_ind.ue_index, [sr_ind](event_logger& ev_logger) { ev_logger.enqueue("sr_ind(ueId={})", sr_ind.ue_index); });
}

void event_manager::handle_ul_bsr(const ul_bsr_indication_message& bsr_ind)
{
  srsran_sanity_check(cell_exists(bsr_ind.cell_index), "Invalid cell index");

  std::lock_guard<std::mutex> lock(common_events.mutex);
  common_events.next_events.emplace_back(
      bsr_ind.ue_index, [bsr_ind](event_logger& ev_logger) { ev_logger.enqueue("sr_ind(ueId={})", bsr_ind.ue_index); });
}

void event_manager::handle_rach_indication(const rach_indication_message& rach_ind)
{
  srsran_sanity_check(cell_exists(rach_ind.cell_index), "Invalid cell index");
  auto& cell = *events_per_cell_list[rach_ind.cell_index];

  std::lock_guard<std::mutex> lock(cell.mutex);
  cell.next_events.emplace_back(MAX_NOF_DU_UES, [this, rach_ind](event_logger& ev_logger) {
    cells[rach_ind.cell_index].ra_sch.handle_rach_indication(rach_ind);
    ev_logger.enqueue("rach_ind(tc-rnti={:#x})", rach_ind.crnti);
  });
}

void event_manager::process_common(slot_point sl_tx)
{
  event_logger ev_logger{MAX_NOF_DU_CELLS, logger};

  common_events.current_events.clear();
  {
    std::lock_guard<std::mutex> lock(common_events.mutex);
    if (last_sl_tx == sl_tx) {
      // In this case, the first thread has finished processing all common events.
      return;
    }
    last_sl_tx = sl_tx;
    common_events.next_events.swap(common_events.current_events);

    // Process common events for UEs with CA enabled.
    for (event_t& ev : common_events.next_events) {
      if (event_requires_sync(ev, false)) {
        ev.callback(ev_logger);
      }
    }
  }

  // Process common events for UEs without CA.
  for (event_t& ev : common_events.next_events) {
    if (not event_requires_sync(ev, true)) {
      ev.callback(ev_logger);
    }
  }
}

void event_manager::run(slot_point sl_tx, du_cell_index_t cell_index)
{
  srsran_sanity_check(cell_exists(cell_index), "Invalid cell index {}", cell_index);

  // Process common events, if not yet processed.
  process_common(sl_tx);

  // Pop pending cell-specific events.
  event_list& cell_events = *events_per_cell_list[cell_index];
  cell_events.current_events.clear();
  {
    std::lock_guard<std::mutex> lock(cell_events.mutex);
    cell_events.next_events.swap(cell_events.current_events);
  }

  // Process cell-specific events.
  event_logger ev_logger{cell_index, logger};
  for (event_t& ev : cell_events.current_events) {
    if (is_du_ue_index_valid(ev.ue_index)) {
      if (not ue_db.contains(ev.ue_index)) {
        log_invalid_ue_index(ev);
        continue;
      }
      ue&         ue    = *ue_db[ev.ue_index];
      ue_carrier* ue_cc = ue.find_cc(cell_index);
      if (ue_cc == nullptr) {
        log_invalid_cc(ev);
        continue;
      }
    }
    ev.callback(ev_logger);
  }
}

bool event_manager::cell_exists(du_cell_index_t cell_index) const
{
  return cell_index < MAX_NOF_DU_CELLS and events_per_cell_list[cell_index] != nullptr;
}

bool event_manager::event_requires_sync(const event_t& ev, bool verbose) const
{
  if (not is_du_ue_index_valid(ev.ue_index)) {
    return false;
  }
  if (not ue_db.contains(ev.ue_index)) {
    if (verbose) {
      log_invalid_ue_index(ev);
    }
    return false;
  }
  return ue_db[ev.ue_index]->is_ca_enabled();
}

void event_manager::log_invalid_ue_index(const event_t& ev) const
{
  logger.warning("SCHED: Event for ueId={} ignored. Cause: UE with provided ueId does not exist", ev.ue_index);
}

void event_manager::log_invalid_cc(const event_t& ev) const
{
  logger.warning("SCHED: Event for ueId={} ignored. Cause: Cell is not configured.", ev.ue_index);
}
