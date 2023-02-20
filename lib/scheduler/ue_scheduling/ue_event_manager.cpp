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
#include "../logging/scheduler_event_logger.h"
#include "../logging/scheduler_metrics_handler.h"

using namespace srsran;

ue_event_manager::ue_event_manager(const scheduler_ue_expert_config& expert_cfg_,
                                   ue_list&                          ue_db_,
                                   sched_configuration_notifier&     mac_notifier_,
                                   scheduler_metrics_handler&        metrics_handler_,
                                   scheduler_event_logger&           ev_logger_) :
  expert_cfg(expert_cfg_),
  ue_db(ue_db_),
  mac_notifier(mac_notifier_),
  metrics_handler(metrics_handler_),
  ev_logger(ev_logger_),
  logger(srslog::fetch_basic_logger("SCHED"))
{
}

void ue_event_manager::handle_ue_creation_request(const sched_ue_creation_request_message& ue_request)
{
  // Create UE object outside the scheduler slot indication handler to minimize latency.
  std::unique_ptr<ue> u =
      std::make_unique<ue>(expert_cfg, *du_cells[ue_request.cfg.cells[0].serv_cell_cfg.cell_index].cfg, ue_request);

  // Defer UE object addition to ue list to the slot indication handler.
  common_events.emplace(MAX_NOF_DU_UES, [this, u = std::move(u)]() mutable {
    // Insert UE in UE repository.
    du_ue_index_t   ueidx       = u->ue_index;
    rnti_t          rnti        = u->crnti;
    du_cell_index_t pcell_index = u->get_pcell().cell_index;
    ue_db.insert(ueidx, std::move(u));
    auto& inserted_ue = ue_db[ueidx];

    // Log Event.
    ev_logger.enqueue(scheduler_event_logger::ue_creation_event{ueidx, rnti, pcell_index});

    // Notify metrics handler.
    metrics_handler.handle_ue_creation(
        inserted_ue.ue_index, inserted_ue.crnti, inserted_ue.get_pcell().cfg().cell_cfg_common.pci);

    // Notify Scheduler UE configuration is complete.
    mac_notifier.on_ue_config_complete(ueidx);
  });
}

void ue_event_manager::handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request)
{
  common_events.emplace(ue_request.ue_index, [this, ue_request]() {
    // Configure existing UE.
    ue_db[ue_request.ue_index].handle_reconfiguration_request(ue_request);

    // Log event.
    ev_logger.enqueue(ue_request);

    // Notify Scheduler UE configuration is complete.
    mac_notifier.on_ue_config_complete(ue_request.ue_index);
  });
}

void ue_event_manager::handle_ue_removal_request(du_ue_index_t ue_index)
{
  common_events.emplace(ue_index, [this, ue_index]() {
    if (not ue_db.contains(ue_index)) {
      logger.warning("Received request to delete ue={} that does not exist", ue_index);
    }
    rnti_t rnti = ue_db[ue_index].crnti;

    // Remove UE from repository.
    ue_db.erase(ue_index);

    // Log event.
    ev_logger.enqueue(sched_ue_delete_message{ue_index, rnti});

    // Notify metrics.
    metrics_handler.handle_ue_deletion(ue_index);

    // Notify Scheduler UE configuration is complete.
    mac_notifier.on_ue_delete_response(ue_index);
  });
}

void ue_event_manager::handle_ul_bsr_indication(const ul_bsr_indication_message& bsr_ind)
{
  srsgnb_sanity_check(cell_exists(bsr_ind.cell_index), "Invalid cell index");

  common_events.emplace(bsr_ind.ue_index, [this, bsr_ind]() {
    if (not ue_db.contains(bsr_ind.ue_index)) {
      logger.warning("BSR received for inexistent ue={}", bsr_ind.ue_index);
      return;
    }
    auto& u = ue_db[bsr_ind.ue_index];
    // Handle event.
    u.handle_bsr_indication(bsr_ind);

    // Log event.
    if (ev_logger.enabled()) {
      scheduler_event_logger::bsr_event event{};
      event.ue_index             = bsr_ind.ue_index;
      event.rnti                 = bsr_ind.crnti;
      event.type                 = bsr_ind.type;
      event.reported_lcgs        = bsr_ind.reported_lcgs;
      event.tot_ul_pending_bytes = units::bytes{u.pending_ul_newtx_bytes()};
      ev_logger.enqueue(event);
    }

    // Notify metrics handler.
    metrics_handler.handle_ul_bsr_indication(bsr_ind);
  });
}

void ue_event_manager::handle_crc_indication(const ul_crc_indication& crc_ind)
{
  srsgnb_assert(cell_exists(crc_ind.cell_index), "Invalid cell index");

  for (unsigned i = 0; i != crc_ind.crcs.size(); ++i) {
    cell_specific_events[crc_ind.cell_index].emplace(
        crc_ind.crcs[i].ue_index, [this, sl_rx = crc_ind.sl_rx, crc = crc_ind.crcs[i]](ue_cell& ue_cc) {
          const int tbs = ue_cc.handle_crc_pdu(sl_rx, crc);
          if (tbs < 0) {
            return;
          }

          // Log event.
          ev_logger.enqueue(scheduler_event_logger::crc_event{
              crc.ue_index, crc.rnti, ue_cc.cell_index, sl_rx, crc.harq_id, crc.tb_crc_success, crc.ul_sinr_metric});

          // Notify metrics handler.
          metrics_handler.handle_crc_indication(crc);
        });
  }
}

void ue_event_manager::handle_harq_ind(ue_cell&                               ue_cc,
                                       slot_point                             uci_sl,
                                       span<const mac_harq_ack_report_status> harq_bits)
{
  for (unsigned dai = 0; dai != harq_bits.size(); ++dai) {
    const mac_harq_ack_report_status ack_value = harq_bits[dai];
    const dl_harq_process*           h_dl      = ue_cc.harqs.dl_ack_info(uci_sl, ack_value, dai);
    if (h_dl != nullptr) {
      // Log Event.
      ev_logger.enqueue(
          scheduler_event_logger::harq_ack_event{ue_cc.ue_index,
                                                 ue_cc.rnti(),
                                                 ue_cc.cell_index,
                                                 uci_sl,
                                                 h_dl->id,
                                                 ack_value,
                                                 units::bytes{h_dl->last_alloc_params().tb[0]->tbs_bytes}});

      if (ack_value != mac_harq_ack_report_status::dtx) {
        // Notify metric.
        metrics_handler.handle_dl_harq_ack(ue_cc.ue_index, ack_value == mac_harq_ack_report_status::ack);
      }
    }
  }
}

void ue_event_manager::handle_uci_indication(const uci_indication& ind)
{
  srsgnb_sanity_check(cell_exists(ind.cell_index), "Invalid cell index");

  for (unsigned i = 0; i != ind.ucis.size(); ++i) {
    if (variant_holds_alternative<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>(ind.ucis[i].pdu)) {
      const auto& pdu = variant_get<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>(ind.ucis[i].pdu);
      // Process DL HARQ ACKs.
      if (not pdu.harqs.empty()) {
        cell_specific_events[ind.cell_index].emplace(
            ind.ucis[i].ue_index, [this, uci_sl = ind.slot_rx, harq_bits = pdu.harqs](ue_cell& ue_cc) {
              handle_harq_ind(ue_cc, uci_sl, harq_bits);
            });
      }

      // Process SRs.
      if (pdu.sr_detected) {
        common_events.emplace(ind.ucis[i].ue_index, [ue_index = ind.ucis[i].ue_index, this]() {
          auto& u = ue_db[ue_index];

          // Handle SR indication.
          u.handle_sr_indication();

          // Log SR event.
          ev_logger.enqueue(scheduler_event_logger::sr_event{ue_index, u.crnti});
        });
      }
    } else if (variant_holds_alternative<uci_indication::uci_pdu::uci_pusch_pdu>(ind.ucis[i].pdu)) {
      const auto& pdu = variant_get<uci_indication::uci_pdu::uci_pusch_pdu>(ind.ucis[i].pdu);
      // Process DL HARQ ACKs.
      if (pdu.harqs.size() > 0) {
        cell_specific_events[ind.cell_index].emplace(
            ind.ucis[i].ue_index, [this, uci_sl = ind.slot_rx, harq_bits = pdu.harqs](ue_cell& ue_cc) {
              handle_harq_ind(ue_cc, uci_sl, harq_bits);
            });
      }

      // Process CSI part 1 bits. NOTE: we assume there are only 4 bits, which represent the CQI.
      if (pdu.csi_part1.size() > 0) {
        cell_specific_events[ind.cell_index].emplace(
            ind.ucis[i].ue_index,
            [csi_1_bits = pdu.csi_part1](ue_cell& ue_cc) { ue_cc.set_latest_wb_cqi(csi_1_bits); });

        metrics_handler.handle_csi_report(ind.ucis[i].ue_index, pdu.csi_part1);
      }
    } else if (variant_holds_alternative<uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu>(ind.ucis[i].pdu)) {
      const auto& pdu = variant_get<uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu>(ind.ucis[i].pdu);
      // Process DL HARQ ACKs.
      if (pdu.harqs.size() > 0) {
        cell_specific_events[ind.cell_index].emplace(
            ind.ucis[i].ue_index, [this, uci_sl = ind.slot_rx, harq_bits = pdu.harqs](ue_cell& ue_cc) {
              handle_harq_ind(ue_cc, uci_sl, harq_bits);
            });
      }

      // Process SRs.
      const size_t sr_bit_position_with_1_sr_bit = 0;
      if (pdu.sr_info.size() > 0 and pdu.sr_info.test(sr_bit_position_with_1_sr_bit)) {
        common_events.emplace(ind.ucis[i].ue_index, [ue_index = ind.ucis[i].ue_index, this]() {
          auto& u = ue_db[ue_index];

          // Handle SR indication.
          u.handle_sr_indication();

          // Log SR event.
          ev_logger.enqueue(scheduler_event_logger::sr_event{ue_index, u.crnti});
        });
      }

      // Process CSI part 1 bits. NOTE: we assume there are only 4 bits, which represent the CQI.
      if (pdu.csi_part1.size() > 0) {
        cell_specific_events[ind.cell_index].emplace(
            ind.ucis[i].ue_index,
            [csi_1_bits = pdu.csi_part1](ue_cell& ue_cc) { ue_cc.set_latest_wb_cqi(csi_1_bits); });

        metrics_handler.handle_csi_report(ind.ucis[i].ue_index, pdu.csi_part1);
      }
    }
  }
}

void ue_event_manager::handle_dl_mac_ce_indication(const dl_mac_ce_indication& ce)
{
  common_events.emplace(ce.ue_index, [this, ce]() {
    ue_db[ce.ue_index].handle_dl_mac_ce_indication(ce);

    // Log event.
    ev_logger.enqueue(ce);
  });
}

void ue_event_manager::handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs)
{
  common_events.emplace(bs.ue_index, [this, bs]() {
    ue& u = ue_db[bs.ue_index];

    u.handle_dl_buffer_state_indication(bs);
    if (bs.lcid == LCID_SRB0) {
      // Signal SRB0 scheduler with the new SRB0 buffer state.
      du_cells[u.get_pcell().cell_index].srb0_sched->handle_dl_buffer_state_indication(bs.ue_index);
    }

    // Log event.
    ev_logger.enqueue(bs);
  });
}

void ue_event_manager::process_common(slot_point sl, du_cell_index_t cell_index)
{
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
      ev.callback();
      ev.callback = {};
    } else {
      if (not ue_db.contains(ev.ue_index)) {
        // Can't find UE. Log error.
        log_invalid_ue_index(ev.ue_index);
        ev.callback = {};
        continue;
      }
      if (ue_db[ev.ue_index].get_pcell().cell_index == cell_index) {
        // If we are currently processing PCell.
        ev.callback();
        ev.callback = {};
      }
    }
  }
}

void ue_event_manager::process_cell_specific(du_cell_index_t cell_index)
{
  // Pop and process pending cell-specific events.
  cell_specific_events[cell_index].slot_indication();
  auto events = cell_specific_events[cell_index].get_events();
  for (cell_event_t& ev : events) {
    if (not ue_db.contains(ev.ue_index)) {
      log_invalid_ue_index(ev.ue_index);
      continue;
    }
    ue&      ue    = ue_db[ev.ue_index];
    ue_cell* ue_cc = ue.find_cell(cell_index);
    if (ue_cc == nullptr) {
      log_invalid_cc(ev.ue_index, cell_index);
      continue;
    }
    ev.callback(*ue_cc);
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

void ue_event_manager::add_cell(const cell_configuration& cell_cfg_, ue_srb0_scheduler& srb0_sched)
{
  srsgnb_assert(not cell_exists(cell_cfg_.cell_index), "Overwriting cell configurations not supported");

  du_cells[cell_cfg_.cell_index].cfg        = &cell_cfg_;
  du_cells[cell_cfg_.cell_index].srb0_sched = &srb0_sched;
}

bool ue_event_manager::cell_exists(du_cell_index_t cell_index) const
{
  return cell_index < MAX_NOF_DU_CELLS and du_cells[cell_index].cfg != nullptr;
}

void ue_event_manager::log_invalid_ue_index(du_ue_index_t ue_index) const
{
  logger.warning("Event for ueId={} ignored. Cause: UE with provided ueId does not exist", ue_index);
}

void ue_event_manager::log_invalid_cc(du_ue_index_t ue_index, du_cell_index_t cell_index) const
{
  logger.warning("Event for ueId={} ignored. Cause: Cell {} is not configured.", ue_index, cell_index);
}
