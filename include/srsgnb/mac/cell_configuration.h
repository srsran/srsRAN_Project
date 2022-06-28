
#ifndef SRSGNB_MAC_CELL_CONFIGURATION_H
#define SRSGNB_MAC_CELL_CONFIGURATION_H

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/carrier_configuration.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/ssb_configuration.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/scheduler/bwp_configuration.h"
#include "srsgnb/scheduler/sched_consts.h"
#include "srsgnb/scheduler/scheduler_configurator.h"
#include "srsgnb/scheduler/scheduler_dci.h"

namespace srsgnb {

struct prach_configuration {
  // TODO
};

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
  prach_configuration   prach_cfg;

  /// Embedded scheduler cell configuration request.
  sched_cell_configuration_request_message sched_req;

  /// [Implementation-defined] SIB1 parameters.
  byte_buffer       sib1_payload;

  // TODO: Fill remaining fields
};

} // namespace srsgnb

#endif // SRSGNB_MAC_CELL_CONFIGURATION_H
