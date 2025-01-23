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

#include "srsran/ran/bwp/bwp_configuration.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/scs_specific_carrier.h"
#include "srsran/ran/srs/srs_configuration.h"
#include "srsran/support/async/async_task.h"

namespace srsran::srs_du {

struct du_positioning_info_request {
  du_ue_index_t ue_index;
};

struct srs_carrier {
  uint32_t                          point_a;
  bool                              freq_shift_7p5khz_present;
  std::vector<scs_specific_carrier> ul_ch_bw_per_scs_list;
  bwp_configuration                 ul_bwp_cfg;
  srs_config                        srs_cfg;
  pci_t                             pci;
};

struct du_positioning_info_response {
  std::vector<srs_carrier> srs_carriers;
};

/// Interface used by F1AP-DU to request the DU for positioning services.
class f1ap_du_positioning_handler
{
public:
  virtual ~f1ap_du_positioning_handler() = default;

  /// Request UE positioning information as per TS 38.473, Section 8.13.9.
  virtual async_task<du_positioning_info_response> request_positioning_info(const du_positioning_info_request& req) = 0;
};

} // namespace srsran::srs_du
