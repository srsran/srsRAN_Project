/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/du/du_high/du_manager/du_manager_params.h"
#include "srsran/f1ap/du/f1ap_du_connection_manager.h"

namespace srsran {
namespace srs_du {

/// \brief Derive packed cell measurementTimingConfiguration from DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \return byte buffer with packed cell measurementTimingConfiguration as per TS38.331.
byte_buffer make_asn1_meas_time_cfg_buffer(const du_cell_config& du_cfg);

struct f1_setup_request_message;

/// \brief Derive the served Cell Information from the DU cell configuration.
du_served_cell_info make_f1ap_du_cell_info(const du_cell_config& du_cfg);

/// \brief Derive the served Cell System Information from the DU cell configuration.
gnb_du_sys_info make_f1ap_du_sys_info(const du_cell_config& du_cfg, std::string* js_str = nullptr);

/// \brief Generate request message for F1AP to initiate an F1 Setup Request procedure.
void fill_f1_setup_request(f1_setup_request_message& req, const du_manager_params::ran_params& ran_params);

} // namespace srs_du
} // namespace srsran
