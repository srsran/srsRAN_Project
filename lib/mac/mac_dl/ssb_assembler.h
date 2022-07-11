/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_MAC_MAC_DL_SSB_ASSEMBLER_H
#define SRSGNB_LIB_MAC_MAC_DL_SSB_ASSEMBLER_H

#include "srsgnb/mac/cell_configuration.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/ssb_configuration.h"
#include "srsgnb/scheduler/dmrs.h"
#include <cstddef>

namespace srsgnb {

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

} // namespace srsgnb

#endif // SRSGNB_LIB_MAC_MAC_DL_SSB_ASSEMBLER_H
