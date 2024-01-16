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

#include "srsran/adt/optional.h"
#include "srsran/mac/mac_cell_result.h"
#include <vector>

namespace srsran {

/// MAC PDSCH precoding information.
struct mac_pdsch_precoding_info {
  /// \brief CSI-RS report.
  ///
  /// This field is empty in case of omnidirectional precoding.
  optional<csi_report_pmi> report;
};

struct mac_pdcch_precoding_info {};
struct mac_ssb_precoding_info {};
struct mac_csi_rs_precoding_info {};

namespace fapi_adaptor {

/// Precoding matrix mapper codebook offset configuration.
struct precoding_matrix_mapper_codebook_offset_configuration {
  /// Codebook offset for the omnidirectional PDSCH.
  unsigned pdsch_omni_offset;
  /// Codebook offsets for SSB. Each entry represents a layer.
  std::vector<unsigned> ssb_codebook_offsets;
  /// Codebook offsets for PDSCH. Each entry represents a layer.
  std::vector<unsigned> pdsch_codebook_offsets;
  /// Codebook offsets for PDCCH. Each entry represents a layer.
  std::vector<unsigned> pdcch_codebook_offsets;
  /// Codebook offsets for CSI-RS. Each entry represents a layer.
  std::vector<unsigned> csi_rs_codebook_offsets;
};

/// \brief Precoding matrix mapper.
///
/// Maps the given arguments to a precoding matrix index.
class precoding_matrix_mapper
{
public:
  precoding_matrix_mapper(unsigned nof_ports_, const precoding_matrix_mapper_codebook_offset_configuration& config);

  /// Maps the given MAC precoding information into a precoding matrix index.
  unsigned map(const mac_pdsch_precoding_info& precoding_info, unsigned nof_layers) const;

  /// Maps the given MAC precoding information into a precoding matrix index.
  unsigned map(const mac_pdcch_precoding_info& precoding_info) const;

  /// Maps the given MAC precoding information into a precoding matrix index.
  unsigned map(const mac_csi_rs_precoding_info& precoding_info) const;

  /// Maps the given MAC precoding information into a precoding matrix index.
  unsigned map(const mac_ssb_precoding_info& precoding_info) const;

private:
  /// Logger.
  srslog::basic_logger& logger;
  /// Number of ports.
  const unsigned nof_ports;
  /// Codebook offset for the omnidirectional PDSCH.
  unsigned pdsch_omni_offset;
  /// Codebook offsets for SSB. Each entry represents a layer.
  std::vector<unsigned> ssb_codebook_offsets;
  /// Codebook offsets for PDSCH. Each entry represents a layer.
  std::vector<unsigned> pdsch_codebook_offsets;
  /// Codebook offsets for PDCCH. Each entry represents a layer.
  std::vector<unsigned> pdcch_codebook_offsets;
  /// Codebook offsets for CSI-RS. Each entry represents a layer.
  std::vector<unsigned> csi_rs_codebook_offsets;
};

} // namespace fapi_adaptor
} // namespace srsran
