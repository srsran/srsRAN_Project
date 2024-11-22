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

#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_logger_registrator.h"
#include "split6_o_du_unit_config.h"

namespace srsran {

/// Registers all the loggers for the O-RAN DU split 6.
inline void register_split6_o_du_loggers(const split6_o_du_unit_config& config)
{
  register_o_du_high_loggers(config.odu_high_cfg);
}

} // namespace srsran
