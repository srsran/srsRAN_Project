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

#include "srsran/mac/cell_configuration.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/ssb_configuration.h"
#include "srsran/scheduler/config/dmrs.h"
#include <cstddef>

namespace srsran {

struct dl_ssb_pdu;
struct ssb_information;

/// This class implements a helper to compute some SSB-specific parameters from the MAC's Cell configuration. These
/// parameters are passed to the scheduler and then used to assemble the SSB message to be sent to the PHY.
class ssb_assembler
{
public:
  explicit ssb_assembler(const mac_cell_creation_request& cell_cfg);

  /// \brief Assemble SSB message to be sent to PHY.
  /// This function fills the SSB msg to send to PHY using parameters from: (i) MAC configuration (general and SSB);(ii)
  /// SSB-specific dependent parameters; (iii) SSB scheduling results.
  /// \param[out] ssb_pdu SSB message to be sent to PHY.
  /// \param[in]  ssb_info SSB scheduling results.
  void assemble_ssb(dl_ssb_pdu& ssb_pdu, const ssb_information& ssb_info);

private:
  /// Cell PCI.
  pci_t pci;
  /// SSB configuration for the cell.
  const ssb_configuration ssb_cfg;
  uint8_t                 pdcch_config_sib1;
  dmrs_typeA_position     dmrs_typeA_pos;
  bool                    cell_barred;
  bool                    intra_f_resel;

  /// Other derived SSB parameters.
  ssb_pattern_case ssb_case;
  uint8_t          L_max;
};

} // namespace srsran
