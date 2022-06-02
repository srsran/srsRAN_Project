
#ifndef SRSGNB_MAC_CELL_CONFIGURATION_H
#define SRSGNB_MAC_CELL_CONFIGURATION_H

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/bwp_configuration.h"
#include "srsgnb/ran/carrier_configuration.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/ssb_configuration.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/scheduler/sched_consts.h"
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
  /// SSB subcarrier spacing.
  /// NOTE: Although this is according to O-RAN WG8, we need to verify if this is the correct SCS.
  subcarrier_spacing ssb_scs;

  carrier_configuration dl_carrier;
  carrier_configuration ul_carrier;
  ssb_configuration     ssb_cfg;
  prach_configuration   prach_cfg;
  /// If present, the cell is in TDD duplex mode.
  optional<tdd_configuration> tdd_cfg;
  /// DL Configuration Common.
  dl_config_common dl_cfg_common;
  ul_config_common ul_cfg_common;

  /// [Implementation-defined] SIB1 parameters.
  /// This is included in MIB message and defines the CORESET 0 and SearchSpaceSet 0. It will be passed to the
  /// scheduler.
  /// NOTE: Although not present in O-RAN WG8, this value must be passed from the DU.
  // TODO: import this value from somewhere (RRC?)
  uint8_t pdcch_config_sib1;
  uint8_t           sib1_mcs;
  uint8_t           sib1_rv;
  aggregation_level sib1_dci_aggr_lev;

  // TODO: Fill remaining fields
};

} // namespace srsgnb

#endif // SRSGNB_MAC_CELL_CONFIGURATION_H
