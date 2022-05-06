
#ifndef SRSGNB_CARRIER_CONFIGURATION_H
#define SRSGNB_CARRIER_CONFIGURATION_H

#include <cstdint>

namespace srsgnb {

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

} // namespace srsgnb

#endif // SRSGNB_CARRIER_CONFIGURATION_H
