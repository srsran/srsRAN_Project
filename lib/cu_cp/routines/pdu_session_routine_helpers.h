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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/pdcp/pdcp_config.h"

namespace srsran {
namespace srs_cu_cp {

void fill_e1ap_drb_pdcp_config(e1ap_pdcp_config& e1ap_pdcp_cfg, const pdcp_config& cu_cp_pdcp_cfg);
void fill_e1ap_qos_flow_param_item(e1ap_qos_flow_qos_param_item&      e1ap_qos_item,
                                   srslog::basic_logger&              logger,
                                   const qos_flow_setup_request_item& request_item);

} // namespace srs_cu_cp
} // namespace srsran
