/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/ran/bwp/bwp_configuration.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/scs_specific_carrier.h"
#include "srsran/ran/srs/srs_configuration.h"
#include "srsran/support/async/async_task.h"

namespace srsran::srs_du {

using trp_id_t = uint32_t;

struct du_trp_info {
  trp_id_t                           trp_id;
  std::optional<pci_t>               pci;
  std::optional<nr_cell_global_id_t> cgi;
  std::optional<uint32_t>            arfcn;
};

struct du_trp_info_response {
  std::vector<du_trp_info> trps;
};

struct du_positioning_info_request {
  du_ue_index_t ue_index;
};

struct srs_carrier {
  uint32_t                          point_a;
  bool                              freq_shift_7p5khz_present;
  std::vector<scs_specific_carrier> ul_ch_bw_per_scs_list;
  bwp_configuration                 ul_bwp_cfg;
  srs_config                        srs_cfg;
  std::optional<pci_t>              pci;
};

struct du_positioning_info_response {
  std::vector<srs_carrier> srs_carriers;
};

struct positioning_meas_quantity {
  /// Granularity factor. Values: (-6, ..., 5).
  std::optional<int8_t> granularity_factor;
};

struct du_trp_meas_request {
  trp_id_t                           trp_id;
  std::optional<nr_cell_global_id_t> cgi;
};

struct du_positioning_meas_request {
  std::vector<du_trp_meas_request>       trp_meas_req_list;
  std::vector<positioning_meas_quantity> pos_meas_quants;
  std::vector<srs_carrier>               srs_carriers;
};

struct pos_meas_result_item {
  uint8_t  granularity;
  uint32_t ul_rtoa;
};

struct pos_meas_result {
  uint32_t                          trp_id;
  std::vector<pos_meas_result_item> results;
};

struct du_positioning_meas_response {
  std::vector<pos_meas_result> pos_meas_list;
};

/// Interface used by F1AP-DU to request the DU for positioning services.
class f1ap_du_positioning_handler
{
public:
  virtual ~f1ap_du_positioning_handler() = default;

  /// Request information regarding supported TRPs as per TS 38.473, Section 8.13.8.
  virtual du_trp_info_response request_trp_info() = 0;

  /// Request UE positioning information as per TS 38.473, Section 8.13.9.
  virtual async_task<du_positioning_info_response> request_positioning_info(const du_positioning_info_request& req) = 0;

  /// Request UE positioning measurement as per TS 38.473, Section 8.13.2.
  virtual async_task<du_positioning_meas_response>
  request_positioning_measurement(const du_positioning_meas_request& req) = 0;
};

} // namespace srsran::srs_du
