
#ifndef SRSGNB_CELL_CONFIGURATION_HELPERS_H
#define SRSGNB_CELL_CONFIGURATION_HELPERS_H

#include "srsgnb/mac/cell_configuration.h"
#include "srsgnb/ran/ssb_mapping.h"

namespace srsgnb {

namespace test_helpers {

carrier_configuration make_default_carrier_configuration()
{
  carrier_configuration cfg;
  cfg.carrier_bw_mhz = 10;
  cfg.arfcn          = 365000;
  cfg.nof_ant        = 1;
  return cfg;
}

mac_cell_configuration make_default_mac_cell_configuration()
{
  mac_cell_configuration cfg;
  cfg.cell_index = to_du_cell_index(0);
  cfg.pci        = 1;
  cfg.scs_common = subcarrier_spacing::kHz15;
  cfg.dl_carrier = make_default_carrier_configuration();
  cfg.ul_carrier = make_default_carrier_configuration();

  cfg.ssb_cfg.ssb_case = ssb_pattern_case::A;
  // TODO: Remaining fields.
  return cfg;
}

} // namespace test_helpers

} // namespace srsgnb

#endif // SRSGNB_CELL_CONFIGURATION_HELPERS_H
