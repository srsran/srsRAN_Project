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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/carrier_configuration.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/ssb_configuration.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/sched_consts.h"
#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/scheduler/scheduler_dci.h"

namespace srsran {

struct tdd_configuration {
  // TODO
};

/// Request to create Cell in MAC and Scheduler.
/// \remark See O-RAN WG8, Section 9.2.1.1.
struct mac_cell_creation_request {
  du_cell_index_t cell_index;
  pci_t           pci;

  /// subcarrierSpacing for common, used for initial access and broadcast message.
  subcarrier_spacing scs_common;

  carrier_configuration dl_carrier;
  carrier_configuration ul_carrier;
  ssb_configuration     ssb_cfg;

  /// Embedded scheduler cell configuration request.
  sched_cell_configuration_request_message sched_req;

  bool cell_barred;
  bool intra_freq_resel;

  /// BCCH-DL-SCH Message payload containing the SIB1 and SI messages to be broadcast.
  std::vector<byte_buffer> bcch_dl_sch_payloads;

  // TODO: Fill remaining fields
};

} // namespace srsran
