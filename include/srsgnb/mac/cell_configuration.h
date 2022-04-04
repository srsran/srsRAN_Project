
#ifndef SRSGNB_MAC_CELL_CONFIGURATION_H
#define SRSGNB_MAC_CELL_CONFIGURATION_H

#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {

/// \brief Configuration of each transmission point associated to the corresponding cell(s). This includes
/// different physical antennas, different frequencies, bandwidths.
struct carrier_configuration {
  /// Width of this carrier in MHz. Values: 5, 10, 15, 20, 25, 30, 40, 50, 60, 70, 80, 90, 100, 200, 400
  uint16_t carrier_bw_mhz;
  /// NR Absolute Radio Frequency Channel Number (NR-ARFCN) point A in kHz. Values: (450000..52600000)
  uint32_t arfcn;
  /// Number of antennas. Values: (0..65355)
  uint16_t nof_ant;
};

struct ssb_configuration {
  // TODO
};

struct prach_configuration {
  // TODO
};

struct tdd_configuration {
  // TODO
};

/// cell configuration
struct mac_cell_configuration {
  du_cell_index_t cell_index;
  pci_t           pci;
  /// subcarrierSpacing for common, used for initial access and broadcast message.
  subcarrier_spacing    scs_common;
  carrier_configuration dl_carrier;
  carrier_configuration ul_carrier;
  ssb_configuration     ssb_cfg;
  prach_configuration   prach_cfg;
  /// If present, the cell is in TDD duplex mode.
  optional<tdd_configuration> tdd_cfg;
  // TODO: Fill remaining fields
};

} // namespace srsgnb

#endif // SRSGNB_MAC_CELL_CONFIGURATION_H
