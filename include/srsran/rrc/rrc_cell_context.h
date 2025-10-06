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
#include "srsran/ran/band_helper.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/tac.h"
#include <cstdint>

namespace srsran {

namespace srs_cu_cp {

/// \brief RRC timers, see TS 38.331 section 7.1.1.
struct rrc_timers_t {
  /// T300: RRC Connection Establishment timer in ms. The timer starts upon transmission of RRCSetupRequest.
  std::chrono::milliseconds t300;
  /// T301: RRC Connection Re-establishment timer in ms. The timer starts upon transmission of
  /// RRCReestablishmentRequest.
  std::chrono::milliseconds t301;
  /// T310: Out-of-sync timer in ms. The timer starts upon detecting physical layer problems for the SpCell i.e. upon
  /// receiving N310 consecutive out-of-sync indications from lower layers.
  std::chrono::milliseconds t310;
  /// T311: RRC Connection Re-establishment procedure timer in ms. The timer starts upon initiating the RRC connection
  /// re-establishment procedure.
  std::chrono::milliseconds t311;
  /// T304: RRC Reconfiguration timer in ms. The UE starts this timer upon receiving the RRC Reconfiguration including
  /// reconfigurationWithSync.
  std::optional<std::chrono::milliseconds> t304;
};

// Cell-related configuration used by the RRC UE.
struct rrc_cell_context {
  nr_cell_global_id_t        cgi;
  tac_t                      tac;
  pci_t                      pci;
  unsigned                   ssb_arfcn; ///< Absolute SSB position.
  std::vector<nr_band>       bands;     ///< Required for capability band filter.
  rrc_timers_t               timers;
  std::vector<plmn_identity> plmn_identity_list; ///< PLMN identities broadcasted in SIB1.
};

} // namespace srs_cu_cp

} // namespace srsran
