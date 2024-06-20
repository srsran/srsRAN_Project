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

#include "srsran/f1ap/cu_cp/du_setup_notifier.h"
#include "srsran/ran/gnb_du_id.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {
namespace srs_cu_cp {

struct du_configuration_context {
  gnb_du_id_t                             id;
  std::string                             name;
  uint8_t                                 rrc_version = 2;
  std::vector<cu_cp_du_served_cells_item> gnb_du_served_cells_list;
};

class du_configuration_handler
{
public:
  virtual ~du_configuration_handler() = default;

  /// \brief Whether the DU already shared its configuration with the CU-CP.
  bool has_context() const { return ctxt != nullptr; }

  /// Getter for the current DU configuration.
  const du_configuration_context& get_context() const
  {
    srsran_assert(ctxt != nullptr, "bad access to DU configuration context");
    return *ctxt;
  }

  /// Add a new DU configuration the CU-CP.
  virtual bool handle_new_du_config(const du_setup_request& req) = 0;

  /// Update the configuration of an existing DU managed by the CU-CP.
  virtual bool handle_du_config_update(const du_config_update_request& req) = 0;

protected:
  const du_configuration_context* ctxt = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran