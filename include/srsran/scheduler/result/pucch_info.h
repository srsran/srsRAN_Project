/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/pucch/pucch_mapping.h"
#include "srsran/scheduler/result/pucch_format.h"

namespace srsran {

/// Info about PUCCH used resource.
struct pucch_info {
  /// This information only is used by the scheduler and not passed to the PHY.
  struct context {
    /// Identifier of the PUCCH PDU within the list of PUCCH PDUs for a given slot. The ID is only meaningful for a
    /// given UE; i.e., different UEs can reuse the same ID, but a UE cannot reuse the same ID for different PDUs.
    unsigned id = MAX_PUCCH_PDUS_PER_SLOT;
    /// Determines whether the PUCCH PDU uses common resources.
    bool is_common = false;
  };

  rnti_t                   crnti;
  const bwp_configuration* bwp_cfg;
  pucch_format             format;
  /// PRBs and symbols for this PUCCH resource.
  pucch_resources resources;
  union {
    pucch_format_0 format_0;
    pucch_format_1 format_1;
    pucch_format_2 format_2;
    pucch_format_3 format_3;
    pucch_format_4 format_4;
  };
  /// In case the PUCCH will contain CSI bits, this struct contains information how those bits are to be decoded.
  std::optional<csi_report_configuration> csi_rep_cfg;

  context pdu_context;
};

} // namespace srsran