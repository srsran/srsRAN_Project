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

#pragma once

#include "du_configuration_handler.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace srs_cu_cp {

/// Validator and repository of configurations for DUs handled by the CU-CP.
class du_configuration_manager
{
public:
  du_configuration_manager(const gnb_id_t& gnb_id, const plmn_identity& plmn_id);

  /// Create a new DU configuration handler.
  std::unique_ptr<du_configuration_handler> create_du_handler();

  size_t nof_dus() const { return dus.size(); }

private:
  class du_configuration_handler_impl;

  using validation_result = error_type<du_setup_result::rejected>;

  expected<const du_configuration_context*, du_setup_result::rejected> add_du_config(const du_setup_request& req);
  expected<const du_configuration_context*, du_setup_result::rejected>
       handle_du_config_update(const du_configuration_context& current_ctxt, const du_config_update_request& req);
  void rem_du(gnb_du_id_t du_id);

  validation_result validate_new_du_config(const du_setup_request& req) const;
  validation_result validate_du_config_update(const du_config_update_request& req) const;
  validation_result validate_cell_config_request(const cu_cp_du_served_cells_item& served_cell) const;

  const gnb_id_t        gnb_id;
  const plmn_identity   plmn_id;
  srslog::basic_logger& logger;

  std::unordered_map<gnb_du_id_t, du_configuration_context> dus;
};

} // namespace srs_cu_cp
} // namespace srsran