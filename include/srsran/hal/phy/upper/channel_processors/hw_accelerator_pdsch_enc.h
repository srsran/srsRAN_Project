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

/// \file
/// \brief Hardware accelerated PDSCH encoder functions declaration.

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/hal/hw_accelerator.h"
#include "srsran/ran/sch/ldpc_base_graph.h"
#include "srsran/ran/sch/modulation_scheme.h"

namespace srsran {
namespace hal {

/// Hardware-accelerated PDSCH encoder configuration parameters.
struct hw_pdsch_encoder_configuration {
  /// Transport Block size expressed as an integer number of bits.
  unsigned nof_tb_bits;
  /// Transport Block CRC size expressed as an integer number of bits.
  unsigned nof_tb_crc_bits;
  /// Base graph used for encoding/decoding the current transport block.
  ldpc_base_graph_type base_graph_index;
  /// Modulation scheme.
  modulation_scheme modulation;
  /// Number of segments in the transport block.
  unsigned nof_segments;
  /// Number of segments that will have a short rate-matched length.
  unsigned nof_short_segments;
  /// Redundancy version, values in {0, 1, 2, 3}.
  unsigned rv;
  /// Total codeword length for short rate-matched segments in bits (Ea).
  unsigned cw_length_a;
  /// Total codeword length for large rate-matched segments in bits (Eb).
  unsigned cw_length_b;
  /// LDPC lifting size (z_c), as per TS38.212, Section 5.2.2.
  unsigned lifting_size;
  /// Length of the circular buffer in bits.
  unsigned Ncb;
  /// Limited buffer rate matching length in bits.
  unsigned Nref;
  /// Segment size expressed as an integer number of bits.
  unsigned nof_segment_bits;
  /// Number of Filler bits (n_filler = K – K’, as per TS38.212 Section 5.2.2.
  unsigned nof_filler_bits;
  /// Total codeword length after rate-matching in bits (E).
  unsigned rm_length;
  /// TB CRC bits.
  static_vector<uint8_t, 3> tb_crc;
  /// Operation mode of the PDSCH encoder (CB = true, TB = false [default]).
  bool cb_mode = false;
};

/// Generic hardware accelerated PDSCH encoder functions.
class hw_accelerator_pdsch_enc : public hw_accelerator<uint8_t, uint8_t>
{
public:
  /// Default destructor.
  virtual ~hw_accelerator_pdsch_enc() = default;

  /// Configures encoding operation given the common HW-oriented PDSCH encoder configuration.
  /// \param[in] config   Structure providing the configuration parameters of the PDSCH encoder.
  /// \param[in] cb_index Optional. Index of the CB for which the PDSCH encoding operation is being configured.
  virtual void configure_operation(const hw_pdsch_encoder_configuration& config, unsigned cb_index = 0) = 0;
};

} // namespace hal
} // namespace srsran
