/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/rrc/meas_types.h"

namespace srsran {
namespace srs_cu_cp {

struct meas_context_t {
  meas_obj_id_t   meas_obj_id   = meas_obj_id_t::invalid;
  report_cfg_id_t report_cfg_id = report_cfg_id_t::invalid;
};

} // namespace srs_cu_cp
} // namespace srsran
