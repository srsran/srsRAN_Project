/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_event_manager.h"
#include "../../../lib/ran/gnb_format.h"

using namespace srsgnb;

/// Class used to enqueue all processed events in a slot and log them in a single batch.
class srsgnb::event_logger
{
public:
  event_logger(du_cell_index_t cell_index_, srslog::basic_logger& logger_) :
    cell_index(cell_index_), enabled(logger_.info.enabled()), logger(logger_)
  {
  }
  event_logger(const event_logger&)            = delete;
  event_logger(event_logger&&)                 = delete;
  event_logger& operator=(const event_logger&) = delete;
  event_logger& operator=(event_logger&&)      = delete;
  ~event_logger()
  {
    if (enabled and fmtbuf.size() > 0) {
      if (cell_index < MAX_NOF_DU_CELLS) {
        logger.info("SCHED: Processed events, cell_index={}: [{}]", cell_index, to_c_str(fmtbuf));
      } else {
        logger.info("SCHED: Processed events: [{}]", to_c_str(fmtbuf));
      }
    }
  }

  template <typename... Args>
  void enqueue(const char* fmtstr, Args&&... args)
  {
    if (enabled) {
      if (fmtbuf.size() > 0) {
        fmt::format_to(fmtbuf, ", ");
      }
      fmt::format_to(fmtbuf, fmtstr, std::forward<Args>(args)...);
    }
  }

private:
  du_cell_index_t       cell_index;
  bool                  enabled;
  srslog::basic_logger& logger;
  fmt::memory_buffer    fmtbuf;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue_event_manager::ue_event_manager(ue_list& ue_db_, sched_configuration_notifier& mac_notifier_) :
  ue_db(ue_db_), mac_notifier(mac_notifier_), logger(srslog::fetch_basic_logger("MAC"))
{
}

void ue_event_manager::handle_add_ue_request(const sched_ue_creation_request_message& ue_request)
{
  // Create UE object outside the scheduler slot indication handler to minimize latency.
  std::unique_ptr<ue> u = std::make_unique<ue>(*cells[ue_request.pcell_index], ue_request);

  // Defer UE object addition to ue list to the slot indication handler.
  common_events.emplace(MAX_NOF_DU_UES, [this, u = std::move(u)](event_logger& ev_logger) mutable {
    ev_logger.enqueue("ue_add(ueId={})", u->ue_index);
    log_ue_proc_event(logger.info, ue_event_prefix{} | u->ue_index, "Sched UE Configuration", "started.");

    // Insert UE in UE repository.
    du_ue_index_t ueidx = u->ue_index;
    ue_db.insert(ueidx, std::move(u));

    log_ue_proc_event(logger.info, ue_event_prefix{} | ueidx, "Sched UE Configuration", "completed.");
    // Notify Scheduler UE configuration is complete.
    mac_notifier.on_ue_config_complete(ueidx);
  });
}

void ue_event_manager::handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request)
{
  common_events.emplace(ue_request.ue_index, [this, ue_request = std::move(ue_request)](event_logger& ev_logger) {
    ev_logger.enqueue("ue_cfg(ueId={})", ue_request.ue_index);
    log_ue_proc_event(logger.info, ue_event_prefix{} | ue_request.ue_index, "Sched UE Reconfiguration", "started.");

    // Configure existing UE.
    ue_db[ue_request.ue_index].handle_reconfiguration_request(ue_request);

    log_ue_proc_event(logger.info, ue_event_prefix{} | ue_request.ue_index, "Sched UE Reconfiguration", "completed.");

    // Notify Scheduler UE configuration is complete.
    mac_notifier.on_ue_config_complete(ue_request.ue_index);
  });
}

void ue_event_manager::handle_ue_delete_request(du_ue_index_t ue_index)
{
  common_events.emplace(ue_index, [this, ue_index](event_logger& ev_logger) {
    ev_logger.enqueue("ue_rem(ueId={})", ue_index);
    log_ue_proc_event(logger.info, ue_event_prefix{} | ue_index, "Sched UE Deletion", "started.");

    // Remove UE from repository.
    ue_db.erase(ue_index);

    log_ue_proc_event(logger.info, ue_event_prefix{} | ue_index, "Sched UE Deletion", "completed.");

    // Notify Scheduler UE configuration is complete.
    mac_notifier.on_ue_delete_response(ue_index);
  });
}

void ue_event_manager::handle_sr_indication(const sr_indication_message& sr_ind)
{
  srsgnb_sanity_check(cell_exists(sr_ind.cell_index), "Invalid cell index");

  common_events.emplace(sr_ind.ue_index, [this, sr_ind](event_logger& ev_logger) {
    ev_logger.enqueue("sr_ind(ueId={})", sr_ind.ue_index);
    ue_db[sr_ind.ue_index].handle_sr_indication(sr_ind);
  });
}

void ue_event_manager::handle_ul_bsr_indication(const ul_bsr_indication_message& bsr_ind)
{
  srsgnb_sanity_check(cell_exists(bsr_ind.cell_index), "Invalid cell index");

  common_events.emplace(bsr_ind.ue_index, [this, bsr_ind](event_logger& ev_logger) {
    ev_logger.enqueue("ul_bsr(ueId={})", bsr_ind.ue_index);
    ue_db[bsr_ind.ue_index].handle_bsr_indication(bsr_ind);
  });
}

void ue_event_manager::handle_crc_indication(const ul_crc_indication& crc_ind)
{
  srsgnb_sanity_check(cell_exists(crc_ind.cell_index), "Invalid cell index");

  for (unsigned i = 0; i != crc_ind.crcs.size(); ++i) {
    cell_specific_events[crc_ind.cell_index].emplace(
        crc_ind.crcs[i].ue_index, [crc = crc_ind.crcs[i]](ue_carrier& ue_cc, event_logger& ev_logger) {
          ev_logger.enqueue("crc(ueId={},h_id={},value={})", crc.ue_index, crc.harq_id, crc.tb_crc_success);
          // TODO: Derive TB.
          ue_cc.harqs.ul_harq(crc.harq_id).ack_info(0, crc.tb_crc_success);
        });
  }
}

void ue_event_manager::handle_dl_mac_ce_indication(const dl_mac_ce_indication& ce)
{
  common_events.emplace(ce.ue_index, [this, ce](event_logger& ev_logger) {
    ev_logger.enqueue("mac_ce(ueId={},ce={})", ce.ue_index, ce.ce_lcid);
    ue_db[ce.ue_index].handle_dl_mac_ce_indication(ce);
  });
}

void ue_event_manager::process_common(slot_point sl, du_cell_index_t cell_index)
{
  event_logger ev_logger{MAX_NOF_DU_CELLS, logger};

  if (last_sl != sl) {
    // Pop pending common events.
    common_events.slot_indication();
    last_sl = sl;
  }

  // Process events for UEs whose PCell matches cell_index argument.
  span<common_event_t> events_to_process = common_events.get_events();
  for (common_event_t& ev : events_to_process) {
    if (ev.callback.is_empty()) {
      // Event already processed.
      continue;
    }
    if (ev.ue_index == MAX_NOF_DU_UES) {
      // The UE is being created.
      ev.callback(ev_logger);
      ev.callback = {};
    } else {
      if (not ue_db.contains(ev.ue_index)) {
        // Can't find UE. Log error.
        log_invalid_ue_index(ev.ue_index);
        ev.callback = {};
        continue;
      }
      if (ue_db[ev.ue_index].ue_cell_indexes()[0] == cell_index) {
        // If we are currently processing PCell.
        ev.callback(ev_logger);
        ev.callback = {};
      }
    }
  }
}

void ue_event_manager::process_cell_specific(du_cell_index_t cell_index)
{
  event_logger ev_logger{cell_index, logger};

  // Pop and process pending cell-specific events.
  cell_specific_events[cell_index].slot_indication();
  auto events = cell_specific_events[cell_index].get_events();
  for (carrier_event_t& ev : events) {
    if (not ue_db.contains(ev.ue_index)) {
      log_invalid_ue_index(ev.ue_index);
      continue;
    }
    ue&         ue    = ue_db[ev.ue_index];
    ue_carrier* ue_cc = ue.find_cc(cell_index);
    if (ue_cc == nullptr) {
      log_invalid_cc(ev.ue_index, cell_index);
      continue;
    }
    ev.callback(*ue_cc, ev_logger);
  }
}

void ue_event_manager::run(slot_point sl, du_cell_index_t cell_index)
{
  srsgnb_sanity_check(cell_exists(cell_index), "Invalid cell index {}", cell_index);

  // Process common events.
  process_common(sl, cell_index);

  // Process carrier specific events.
  process_cell_specific(cell_index);
}

void ue_event_manager::add_cell_config(const cell_configuration& cell_cfg_)
{
  srsgnb_assert(not cell_exists(cell_cfg_.cell_index), "Overwriting cell configurations not supported");

  cells[cell_cfg_.cell_index] = &cell_cfg_;
}

bool ue_event_manager::cell_exists(du_cell_index_t cell_index) const
{
  return cell_index < MAX_NOF_DU_CELLS and cells[cell_index] != nullptr;
}

void ue_event_manager::log_invalid_ue_index(du_ue_index_t ue_index) const
{
  logger.warning("SCHED: Event for ueId={} ignored. Cause: UE with provided ueId does not exist", ue_index);
}

void ue_event_manager::log_invalid_cc(du_ue_index_t ue_index, du_cell_index_t cell_index) const
{
  logger.warning("SCHED: Event for ueId={} ignored. Cause: Cell {} is not configured.", ue_index, cell_index);
}
