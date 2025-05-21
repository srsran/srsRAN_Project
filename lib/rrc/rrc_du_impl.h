/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "metrics/rrc_du_metrics_aggregator.h"
#include "ue/rrc_ue_impl.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/rrc/rrc_config.h"
#include "srsran/rrc/rrc_du.h"
#include <unordered_map>

namespace srsran {

namespace srs_cu_cp {

/// Adapter between RRC UE and RRC DU.
class rrc_ue_rrc_du_adapter : public rrc_ue_event_notifier
{
public:
  explicit rrc_ue_rrc_du_adapter(rrc_du_connection_event_handler& metrics_handler_) : metrics_handler(metrics_handler_)
  {
  }

  void on_new_rrc_connection() override { metrics_handler.handle_successful_rrc_setup(); }

  void on_successful_rrc_release() override { metrics_handler.handle_successful_rrc_release(); }

  void on_attempted_rrc_connection_establishment(establishment_cause_t cause) override
  {
    metrics_handler.handle_attempted_rrc_setup(cause);
  }

  void on_successful_rrc_connection_establishment(establishment_cause_t cause) override
  {
    metrics_handler.handle_successful_rrc_setup(cause);
  }

  void on_attempted_rrc_connection_reestablishment() override
  {
    metrics_handler.handle_attempted_rrc_reestablishment();
  }

  void on_successful_rrc_connection_reestablishment() override
  {
    metrics_handler.handle_successful_rrc_reestablishment();
  }

  void on_successful_rrc_connection_reestablishment_fallback() override
  {
    metrics_handler.handle_successful_rrc_reestablishment_fallback();
  }

private:
  rrc_du_connection_event_handler& metrics_handler;
};

/// Main RRC representation with the DU.
class rrc_du_impl : public rrc_du
{
public:
  rrc_du_impl(const rrc_cfg_t& cfg_, rrc_du_measurement_config_notifier& meas_config_notifier_);
  ~rrc_du_impl() = default;

  // rrc_du_cell_manager.
  bool handle_served_cell_list(const std::vector<cu_cp_du_served_cells_item>& served_cell_list) override;

  // rrc_du_ue_repository.
  byte_buffer       get_rrc_reject() override;
  rrc_ue_interface* add_ue(const rrc_ue_creation_message& msg) override;
  void              release_ues() override;
  size_t            get_nof_ues() const override { return ue_db.size(); }

  // rrc_ue_handler.
  rrc_ue_interface* find_ue(ue_index_t ue_index) override
  {
    if (ue_db.find(ue_index) == ue_db.end()) {
      return nullptr;
    }
    return ue_db.at(ue_index).get();
  }
  void remove_ue(ue_index_t ue_index) override;

  // rrc_du_connection_event_handler.
  void handle_successful_rrc_setup(std::optional<establishment_cause_t> cause) override;
  void handle_successful_rrc_release() override;
  void handle_attempted_rrc_setup(establishment_cause_t cause) override;
  void handle_attempted_rrc_reestablishment() override;
  void handle_successful_rrc_reestablishment() override;
  void handle_successful_rrc_reestablishment_fallback() override;

  rrc_du_cell_manager&             get_rrc_du_cell_manager() override { return *this; }
  rrc_du_ue_repository&            get_rrc_du_ue_repository() override { return *this; }
  rrc_ue_handler&                  get_rrc_ue_handler() override { return *this; }
  rrc_du_connection_event_handler& get_rrc_du_connection_event_handler() override { return *this; }
  rrc_du_metrics_collector&        get_rrc_du_metrics_collector() override { return metrics_aggregator; }

private:
  // Helpers.
  const rrc_cfg_t cfg;

  // Notifier to the CU-CP.
  rrc_du_measurement_config_notifier& meas_config_notifier;
  srslog::basic_logger&               logger;

  // RRC UE to RRC DU adapters.
  std::unordered_map<ue_index_t, rrc_ue_rrc_du_adapter> rrc_ue_rrc_du_adapters;

  // RRC-internal user database indexed by ue_index.
  std::unordered_map<ue_index_t, std::unique_ptr<rrc_ue_impl>> ue_db;
  // Cell database to store cell information from the DU.
  std::map<nr_cell_identity, rrc_cell_info> cell_info_db;

  // Metrics aggregator.
  rrc_du_metrics_aggregator metrics_aggregator;
};

} // namespace srs_cu_cp

} // namespace srsran
