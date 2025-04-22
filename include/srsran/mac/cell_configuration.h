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
#include "srsran/ran/carrier_configuration.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/ssb/ssb_configuration.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/scheduler/scheduler_sys_info_handler.h"

namespace srsran {

/// System Information signalled by the cell.
struct mac_cell_sys_info_config {
  /// SIB1 payload.
  byte_buffer sib1;
  /// SI messages provided by the cell and which are part of the SIB1 SI-SchedConfig.
  static_vector<byte_buffer, MAX_SI_MESSAGES> si_messages;
  /// SI scheduling configuration to provide to MAC scheduler.
  si_scheduling_update_request si_sched_cfg;
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

  /// Cell-specific encoded system information.
  mac_cell_sys_info_config sys_info;
};

} // namespace srsran
