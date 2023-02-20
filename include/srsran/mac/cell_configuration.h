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

  /// BCCH-DL-SCH Message payload containing the SIB1 to be broadcast.
  byte_buffer bcch_dl_sch_payload;

  // TODO: Fill remaining fields
};

} // namespace srsran
