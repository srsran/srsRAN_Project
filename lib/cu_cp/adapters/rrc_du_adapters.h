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

#include "../cu_cp_impl_interface.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between RRC DU and CU-CP
class rrc_du_cu_cp_adapter : public rrc_du_measurement_config_notifier
{
public:
  void connect_cu_cp(cu_cp_measurement_config_handler& meas_config_handler_)
  {
    meas_config_handler = &meas_config_handler_;
  }

  bool on_cell_config_update_request(nr_cell_identity nci, const serving_cell_meas_config& serv_cell_cfg) override
  {
    srsran_assert(meas_config_handler != nullptr, "Measurement config handler must not be nullptr");
    return meas_config_handler->handle_cell_config_update_request(nci, serv_cell_cfg);
  }

private:
  cu_cp_measurement_config_handler* meas_config_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
