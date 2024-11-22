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

#include "du_ue_resource_config.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/srslog/logger.h"

namespace srsran::srs_du {

class du_meas_config_manager
{
public:
  du_meas_config_manager(span<const du_cell_config> cell_cfg_list);

  void update(du_ue_resource_config& ue_cfg, const byte_buffer& meas_cfg);

private:
  span<const du_cell_config> cell_cfg_list;
  srslog::basic_logger&      logger;
};

} // namespace srsran::srs_du
