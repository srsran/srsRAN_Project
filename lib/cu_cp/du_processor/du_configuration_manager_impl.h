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

/// Repository of configurations for DUs handled by the CU-CP.
class du_configuration_manager_impl
{
public:
  du_configuration_manager_impl();

  std::unique_ptr<du_configuration_handler> create_du_handler();

private:
  class du_configuration_handler_impl;

  const du_configuration_context* add_du_config(const du_setup_request& req);
  const du_configuration_context* handle_du_config_update(const du_configuration_context& current_ctxt,
                                                          const du_config_update_request& req);
  void                            rem_du(gnb_du_id_t du_id);

  bool validate_new_du_config(const du_setup_request& req) const;
  bool validate_du_config_update(const du_config_update_request& req) const;

  srslog::basic_logger& logger;

  std::unordered_map<gnb_du_id_t, du_configuration_context> dus;
};

} // namespace srs_cu_cp
} // namespace srsran