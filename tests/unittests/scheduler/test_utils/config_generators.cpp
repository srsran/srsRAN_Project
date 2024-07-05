/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "config_generators.h"
#include "lib/scheduler/logging/scheduler_metrics_ue_configurator.h"

using namespace srsran;
using namespace test_helpers;

namespace {

class dummy_sched_configuration_notifier : public sched_configuration_notifier
{
public:
  void on_ue_config_complete(du_ue_index_t ue_index, bool ue_creation_result) override {}
  void on_ue_delete_response(du_ue_index_t ue_index) override {}
};

class dummy_scheduler_ue_metrics_notifier : public scheduler_metrics_notifier
{
public:
  void report_metrics(const scheduler_cell_metrics& metrics) override {}
};

class dummy_sched_metrics_ue_configurator : public sched_metrics_ue_configurator
{
public:
  void handle_ue_creation(du_ue_index_t ue_index,
                          rnti_t        rnti,
                          pci_t         pcell_pci,
                          unsigned      num_prbs,
                          unsigned      num_slots_per_frame) override
  {
  }
  void handle_ue_deletion(du_ue_index_t ue_index) override {}
};

} // namespace

test_sched_config_manager::test_sched_config_manager(const cell_config_builder_params& builder_params_,
                                                     const scheduler_expert_config&    expert_cfg_) :
  builder_params(builder_params_),
  expert_cfg(expert_cfg_),
  cfg_notifier(std::make_unique<dummy_sched_configuration_notifier>()),
  metric_notifier(std::make_unique<dummy_scheduler_ue_metrics_notifier>()),
  ue_metrics_configurator(std::make_unique<dummy_sched_metrics_ue_configurator>()),
  cfg_mng(scheduler_config{expert_cfg, *cfg_notifier, *metric_notifier}, *ue_metrics_configurator)
{
  default_cell_req = test_helpers::make_default_sched_cell_configuration_request(builder_params);
  default_ue_req   = test_helpers::create_default_sched_ue_creation_request(builder_params, {lcid_t::LCID_MIN_DRB});
}

test_sched_config_manager::~test_sched_config_manager() {}

const cell_configuration* test_sched_config_manager::add_cell(const sched_cell_configuration_request_message& msg)
{
  return cfg_mng.add_cell(msg);
}

const ue_configuration* test_sched_config_manager::add_ue(const sched_ue_creation_request_message& cfg_req)
{
  auto ue_ev = cfg_mng.add_ue(cfg_req);
  return ue_ev.valid() ? &ue_ev.next_config() : nullptr;
}

bool test_sched_config_manager::rem_ue(du_ue_index_t ue_index)
{
  auto ev = cfg_mng.remove_ue(ue_index);
  return ev.valid();
}
