/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_impl.h"
#include "common_scheduling/ssb_scheduler.h"
#include "ue_scheduling/ue_scheduler_impl.h"
#include "srsgnb/scheduler/config/scheduler_cell_config_validator.h"
#include "srsgnb/scheduler/config/scheduler_ue_config_validator.h"

using namespace srsgnb;

scheduler_impl::scheduler_impl(const scheduler_config& sched_cfg_) :
  sched_cfg(sched_cfg_.expert_params),
  logger(srslog::fetch_basic_logger("SCHED")),
  metrics(sched_cfg.metrics_report_period, sched_cfg_.metrics_notifier),
  ue_sched(std::make_unique<ue_scheduler_impl>(sched_cfg.ue, sched_cfg_.config_notifier, metrics, sched_ev_logger)),
  ue_cfg_handler(ue_sched->get_ue_configurator()),
  feedback_handler(ue_sched->get_feedback_handler()),
  dl_bs_handler(ue_sched->get_dl_buffer_state_indication_handler()),
  cells(sched_cfg, sched_ev_logger)
{
}

bool scheduler_impl::handle_cell_configuration_request(const sched_cell_configuration_request_message& msg)
{
  srsgnb_assert(not config_validators::validate_sched_cell_configuration_request_message(msg, sched_cfg).is_error(),
                "Invalid cell configuration request message. Cause: {}",
                config_validators::validate_sched_cell_configuration_request_message(msg, sched_cfg).error().c_str());

  cells.add_cell(msg.cell_index, msg);

  ue_sched->add_cell(ue_scheduler_cell_params{msg.cell_index,
                                              &cells[msg.cell_index].pdcch_sch,
                                              &cells[msg.cell_index].pucch_alloc,
                                              &cells[msg.cell_index].uci_alloc,
                                              &cells[msg.cell_index].res_grid});

  logger.info("Cell with cell_index={} was configured.", msg.cell_index);

  return true;
}

void scheduler_impl::handle_ue_creation_request(const sched_ue_creation_request_message& ue_request)
{
  srsgnb_assert(not config_validators::validate_sched_ue_creation_request_message(ue_request).is_error(),
                "Invalid UE creation request message. Cause: {}",
                config_validators::validate_sched_ue_creation_request_message(ue_request).error().c_str());

  ue_cfg_handler.handle_ue_creation_request(ue_request);
}

void scheduler_impl::handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request)
{
  ue_cfg_handler.handle_ue_reconfiguration_request(ue_request);
}

void scheduler_impl::handle_ue_removal_request(du_ue_index_t ue_index)
{
  ue_cfg_handler.handle_ue_removal_request(ue_index);
}

void scheduler_impl::handle_rach_indication(const rach_indication_message& msg)
{
  cells[msg.cell_index].ra_sch.handle_rach_indication(msg);
}

void scheduler_impl::handle_crc_indication(const ul_crc_indication& crc_ind)
{
  bool has_msg3_crcs = std::any_of(
      crc_ind.crcs.begin(), crc_ind.crcs.end(), [](const auto& pdu) { return pdu.ue_index == INVALID_DU_UE_INDEX; });

  if (has_msg3_crcs) {
    ul_crc_indication msg3_crcs{}, ue_crcs{};
    msg3_crcs.sl_rx      = crc_ind.sl_rx;
    msg3_crcs.cell_index = crc_ind.cell_index;
    ue_crcs.sl_rx        = crc_ind.sl_rx;
    ue_crcs.cell_index   = crc_ind.cell_index;
    for (const ul_crc_pdu_indication& crc_pdu : crc_ind.crcs) {
      if (crc_pdu.ue_index == INVALID_DU_UE_INDEX) {
        msg3_crcs.crcs.push_back(crc_pdu);
      } else {
        ue_crcs.crcs.push_back(crc_pdu);
      }
    }
    // Forward CRC to Msg3 HARQs that has no ueId yet associated.
    cells[crc_ind.cell_index].ra_sch.handle_crc_indication(msg3_crcs);
    // Forward remaining CRCs to UE scheduler.
    feedback_handler.handle_crc_indication(ue_crcs);
  } else {
    feedback_handler.handle_crc_indication(crc_ind);
  }
}

void scheduler_impl::handle_uci_indication(const uci_indication& uci)
{
  feedback_handler.handle_uci_indication(uci);
}

const sched_result* scheduler_impl::slot_indication(slot_point sl_tx, du_cell_index_t cell_index)
{
  if (cell_index == to_du_cell_index(0)) {
    // Set scheduler logger context.
    logger.set_context(sl_tx.sfn(), sl_tx.slot_index());
  }

  auto& cell = cells[cell_index];

  // > Reset cell resource grid state.
  cell.slot_indication(sl_tx);

  // > SSB scheduling.
  cell.ssb_sch.run_slot(cell.res_grid, sl_tx);

  // > Schedule DL signalling.
  cell.sib1_sch.schedule_sib1(cell.res_grid[0], sl_tx);

  // > Schedule PUCCH guardbands.
  cell.pucch_guard_sch.run_slot(cell.res_grid);

  // > Schedule PRACH PDUs.
  cell.prach_sch.run_slot(cell.res_grid);

  // > Schedule RARs and Msg3.
  cell.ra_sch.run_slot(cell.res_grid);

  // > Schedule Paging.
  cell.pg_sch.schedule_paging(cell.res_grid);

  // > Schedule UE DL and UL data.
  ue_sched->run_slot(sl_tx, cell_index);

  // > Log processed events.
  sched_ev_logger.log();

  // > Log the scheduler results.
  sched_result_logger.log(cell.res_grid[0].result);

  // > Push the scheduler results to the metrics handler.
  metrics.push_result(sl_tx, cell.res_grid[0].result);

  // Return result for the slot.
  return &cell.res_grid[0].result;
}

void scheduler_impl::handle_paging_indication(const paging_indication_message& pi)
{
  auto& cell = cells[pi.cell_index];
  cell.pg_sch.handle_paging_indication_message(pi);
}
