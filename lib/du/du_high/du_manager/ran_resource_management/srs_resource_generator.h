/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_ue_resource_config.h"

namespace srsran::srs_du {

/// Contains the parameters for the SRS resources of a cell.
struct du_srs_resource {
  /// Id of the cell SRS resource.
  unsigned cell_res_id;
  /// Comb offset, as per \c transmissionComb, \c SRS-Resource, \c SRS-Config, TS 38.331.
  bounded_integer<uint8_t, 0, 4> tx_comb_offset;
  /// OFDM symbol range where the SRS resource is placed.
  ofdm_symbol_range symbols;
  /// \c freqDomainPosition, as per \c SRS-Resource, \c SRS-Config, TS 38.331.
  unsigned freq_dom_position = 0;
  /// \c sequenceId, as per \c SRS-Resource, \c SRS-Config, TS 38.331.
  unsigned sequence_id = 0;
  /// Cyclic shift, as per \c transmissionComb, \c SRS-Resource, \c SRS-Config, TS 38.331.
  unsigned cs = 0;
};

/// \brief Generates the list of orthogonal SRS resources available in a cell.
/// The resources of this cells are meant to be used by the UEs; the same resources can be reused by different UEs over
/// different slots. Note that this function does not allocate the resources to the UEs, it only creates the cell
/// resource list.
/// \param[in] du_cell_cfg Cell configuration parameters.
/// \return List of orthogonal SRS resources.
std::vector<du_srs_resource> generate_cell_srs_list(const du_cell_config& du_cell_cfg);

} // namespace srsran::srs_du
