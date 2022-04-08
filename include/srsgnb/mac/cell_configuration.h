
#ifndef SRSGNB_MAC_CELL_CONFIGURATION_H
#define SRSGNB_MAC_CELL_CONFIGURATION_H

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/mac/sched_consts.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {

enum class ssb_pattern_case;

/// \brief Configuration of each transmission point associated to the corresponding cell(s). This includes
/// different physical antennas, different frequencies, bandwidths.
struct carrier_configuration {
  /// Width of this carrier in MHz. Values: 5, 10, 15, 20, 25, 30, 40, 50, 60, 70, 80, 90, 100, 200, 400.
  uint16_t carrier_bw_mhz;
  /// NR Absolute Radio Frequency Channel Number (NR-ARFCN) point A in kHz. Values: (450000..52600000).
  uint32_t arfcn;
  /// Number of antennas. Values: (0..65355).
  uint16_t nof_ant;
};

/// SSB Configuration.
/// \remark See O-RAN WG8, Section 9.2.1.1, Table 9-3.
struct ssb_configuration {
  /// SSB block power value: (-60dBm, ..., 50dBm) [Table 3-39, FAPI and ss-PBCH-BlockPower in TS 38.311].
  int8_t ssb_power;
  /// SSB subcarrier spacing [Table 4.2-1, TS 38.211]
  /// Possible values: FR1 only {15kHz, 30kHz, 60kHz}, FR2 only {60kHz, 120kHz}.
  subcarrier_spacing scs;
  /// Given in PRBs - Values [0, 2199] - [Table 3-79, FAPI].
  uint16_t ssb_offset_to_point_A;
  /// Values: [0: 5ms, 1: 10ms, 2: 20ms, 3: 40ms, 4: 80ms, 5: ms160].
  uint8_t ssb_period;
  /// SSB SubcarrierOffest or k_ssb - Values [0, 31].
  /// NOTE: According to TS 38.211, Section 7.4.3.1, values are [0, 23], but FAPI and ORAN uses [0, 31].
  uint8_t ssb_subcarrier_offset;
  /// This ssb-PositionsInBurs in the TS 38.311, 0 = SSB beam not used, 1 = SSB beam used.
  bounded_bitset<NOF_BEAMS> ssb_bitmap;
  /// The n-th element of the array indicates what Beam ID to use for the n-th SSB occasion in ssb_bitmap. Only relevant
  /// if n-th bit of ssb_bitmap is 1.
  std::array<uint8_t, NOF_BEAMS> beam_ids;
  /// Values: 0 = 0dB, 1 = 3dB.
  uint8_t beta_pss;

  /// Additional parameter not included in ORAN iface.
  ssb_pattern_case ssb_case;
  // TODO: ssb_case and paired_spectrum could be moved to sched/cell_configuration constructor.
  bool             paired_spectrum;
};

struct prach_configuration {
  // TODO
};

struct tdd_configuration {
  // TODO
};

/// Cell configuration.
/// \remark See O-RAN WG8, Section 9.2.1.1.
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

  /// SSB subcarrier spacing
  /// NOTE: Although this is according to O-RAN WG8, we need to verify if this is the correct SCS.
  subcarrier_spacing ssb_scs;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_CELL_CONFIGURATION_H
