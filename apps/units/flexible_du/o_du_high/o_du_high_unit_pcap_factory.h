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

#include "du_high/pcap_factory.h"
#include "o_du_high_unit_config.h"

namespace srsran {

/// Creates the PCAPs of the DU.
inline flexible_du_pcaps create_o_du_pcaps(const o_du_high_unit_config& unit_cfg, worker_manager& workers)
{
  return create_du_pcaps(unit_cfg.du_high_cfg.config.pcaps, workers);
}

} // namespace srsran
