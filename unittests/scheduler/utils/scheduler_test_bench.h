/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../scheduler_test_suite.h"
#include "lib/scheduler/cell/cell_configuration.h"
#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "srsgnb/scheduler/scheduler_configurator.h"
#include "srsgnb/scheduler/scheduler_factory.h"
#include <random>

namespace srsgnb {

class sched_cfg_dummy_notifier : public sched_configuration_notifier
{
public:
  du_ue_index_t last_ue_config_idx = INVALID_DU_UE_INDEX;
  du_ue_index_t last_ue_delete_idx = INVALID_DU_UE_INDEX;

  void on_ue_config_complete(du_ue_index_t ue_index) override { last_ue_config_idx = ue_index; }
  void on_ue_delete_response(du_ue_index_t ue_index) override { last_ue_delete_idx = ue_index; }
};

std::random_device rd;
std::mt19937       g(rd());

/// Helper class to help setup a scheduler unit test.
class scheduler_test_bench
{
public:
  explicit scheduler_test_bench(unsigned tx_rx_delay_ = 4, subcarrier_spacing max_scs = subcarrier_spacing::kHz15) :
    tx_rx_delay(tx_rx_delay_),
    sched(create_scheduler(notif)),
    next_slot(to_numerology_value(max_scs), std::uniform_int_distribution<unsigned>{0, 10240}(g))
  {
    logger.set_level(srslog::basic_levels::debug);
  }

  slot_point next_slot_rx() const { return next_slot - tx_rx_delay; }

  slot_point last_result_slot() const { return next_slot - 1; }

  void add_cell(const sched_cell_configuration_request_message& cell_cfg_req)
  {
    cell_cfg_list.emplace(cell_cfg_req.cell_index, cell_cfg_req);
    sched->handle_cell_configuration_request(cell_cfg_req);
  }

  void run_slot(du_cell_index_t cell_idx = to_du_cell_index(0))
  {
    srsgnb_assert(cell_cfg_list.size() > cell_idx, "Invalid cellId={}", cell_idx);
    logger.set_context(next_slot.to_uint());
    last_sched_res = sched->slot_indication(next_slot, cell_idx);
    srsgnb_assert(last_sched_res != nullptr, "No scheduler output was provided");
    result_logger.log(*last_sched_res);
    test_scheduler_result_consistency(cell_cfg_list[cell_idx], *last_sched_res);
    ++next_slot;
  }

  const unsigned                 tx_rx_delay;
  srslog::basic_logger&          logger = srslog::fetch_basic_logger("MAC");
  sched_cfg_dummy_notifier       notif;
  std::unique_ptr<mac_scheduler> sched;
  scheduler_result_logger        result_logger;

  slot_array<cell_configuration, MAX_NOF_DU_CELLS> cell_cfg_list;

  slot_point          next_slot;
  const sched_result* last_sched_res = nullptr;
};

} // namespace srsgnb
