/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_configuration_handler.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace srs_cu_cp {

struct rrc_cfg_t;

/// Repository of configurations for DUs handled by the CU-CP.
class du_configuration_manager_impl
{
public:
  du_configuration_manager_impl(const rrc_cfg_t& rrc_cfg_);

  std::unique_ptr<du_configuration_handler> create_du_handler();

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

  const rrc_cfg_t&      rrc_cfg;
  srslog::basic_logger& logger;

  std::unordered_map<gnb_du_id_t, du_configuration_context> dus;
};

} // namespace srs_cu_cp
} // namespace srsran