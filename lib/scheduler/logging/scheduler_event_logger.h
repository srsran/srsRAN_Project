/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/scheduler/mac_scheduler.h"
#include "srsgnb/srslog/srslog.h"
#include "srsgnb/support/format_utils.h"

namespace srsgnb {

class scheduler_event_logger
{
public:
  struct prach_event {
    slot_point      slot_rx;
    du_cell_index_t cell_index;
    unsigned        preamble_id;
    rnti_t          ra_rnti;
    rnti_t          tc_rnti;
    unsigned        ta;
  };
  struct ue_creation_event {
    du_ue_index_t   ue_index;
    rnti_t          rnti;
    du_cell_index_t pcell_index;
  };
  struct crc_event {
    du_ue_index_t   ue_index;
    rnti_t          rnti;
    du_cell_index_t cell_index;
    harq_id_t       h_id;
    bool            crc;
    double          ul_sinr_db;
  };
  struct harq_ack_event {
    du_ue_index_t   ue_index;
    rnti_t          rnti;
    du_cell_index_t cell_index;
    harq_id_t       h_id;
    bool            ack;
    units::bytes    tbs;
  };
  struct sr_event {
    du_ue_index_t ue_index;
    rnti_t        rnti;
  };

  scheduler_event_logger() :
    logger(srslog::fetch_basic_logger("SCHED")),
    mode(logger.debug.enabled() ? mode_t::debug : (logger.info.enabled() ? mode_t::info : mode_t::none))
  {
  }

  void log()
  {
    if (mode == none or fmtbuf.size() == 0) {
      return;
    }
    log_impl();
  }

  template <typename Event>
  void enqueue(Event&& ev)
  {
    if (mode == none) {
      return;
    }
    enqueue_impl(std::forward<Event>(ev));
  }

private:
  enum mode_t { none, info, debug };
  srslog::basic_logger& logger;

  mode_t mode = none;

  const char* separator() const;

  void log_impl();

  void enqueue_impl(const prach_event& rach_ev);
  void enqueue_impl(const rach_indication_message& rach_ind);

  void enqueue_impl(const ue_creation_event& ue_request);
  void enqueue_impl(const sched_ue_reconfiguration_message& ue_request);
  void enqueue_impl(const sched_ue_delete_message& ue_request);

  void enqueue_impl(const sr_event& sr);
  void enqueue_impl(const ul_bsr_indication_message& bsr);
  void enqueue_impl(const harq_ack_event& harq_ev);
  void enqueue_impl(const crc_event& crc_ev);
  void enqueue_impl(const dl_mac_ce_indication& mac_ce);
  void enqueue_impl(const dl_buffer_state_indication_message& bs);

  fmt::memory_buffer fmtbuf;
};

} // namespace srsgnb