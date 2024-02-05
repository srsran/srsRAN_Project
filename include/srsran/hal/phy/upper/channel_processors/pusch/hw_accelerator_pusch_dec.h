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
/// \brief Hardware accelerated PUSCH decoder functions declaration.

#pragma once

#include "srsran/hal/hw_accelerator.h"
#include "srsran/ran/sch/ldpc_base_graph.h"
#include "srsran/ran/sch/modulation_scheme.h"

namespace srsran {
namespace hal {

/// CB CRC types supported by the hardware-accelerated PUSCH decoder.
enum class hw_dec_cb_crc_type : uint8_t { CRC16 = 0, CRC24B = 1, CRC24A = 2 };

/// Hardware-accelerated PUSCH decoder configuration parameters.
struct hw_pusch_decoder_configuration {
  /// Base graph used for encoding/decoding the current transport block.
  ldpc_base_graph_type base_graph_index;
  /// Modulation scheme.
  modulation_scheme modulation;
  /// Number of segments in the transport block.
  unsigned nof_segments;
  /// Redundancy version, values in {0, 1, 2, 3}.
  unsigned rv;
  /// Total codeword length in bits (E).
  unsigned cw_length;
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
  /// Maximum number of iterations of the LDPC decoder.
  unsigned max_nof_ldpc_iterations;
  /// Flag for LDPC decoder early stopping: \c true to activate.
  bool use_early_stop;
  /// Flag to denote new data (first HARQ transmission).
  bool new_data;
  /// CB CRC length in bits.
  unsigned cb_crc_len;
  /// CB CRC type.
  hw_dec_cb_crc_type cb_crc_type;
  /// Unique absolute CB ID for the HARQ buffer utilized by the decoding task.
  unsigned absolute_cb_id;
};

/// Hardware-accelerated PUSCH decoder output parameters.
struct hw_pusch_decoder_outputs {
  /// CRC value of the current decoding LDPC decoder operation.
  bool CRC_pass;
  /// Number of used iterations in the current decoding LDPC decoder operation.
  unsigned nof_ldpc_iterations;
};

/// Generic hardware accelerated PUSCH decoder functions.
class hw_accelerator_pusch_dec : public hw_accelerator<int8_t, uint8_t>
{
public:
  /// Default destructor.
  virtual ~hw_accelerator_pusch_dec() = default;

  /// Configures the decoding operation given the common HW-oriented PUSCH decoder configuration.
  /// \param[in] config   Structure providing the configuration parameters of the PUSCH decoder.
  /// \param[in] cb_index Optional. Index of the CB for which the PUSCH decoding operation is being configured.
  virtual void configure_operation(const hw_pusch_decoder_configuration& config, unsigned cb_index = 0) = 0;

  /// Reads the decoding operation status outputs.
  /// \param[out] out            Output results associated to the PUSCH decoding operation.
  /// \param[in]  cb_index       Optional. Index of the decoded CB.
  /// \param[in]  absolute_cb_id Optional. Unique ID of associated to the CB in the soft-buffer and HARQ ACK context
  /// entry.
  virtual void
  read_operation_outputs(hw_pusch_decoder_outputs& out, unsigned cb_index = 0, unsigned absolute_cb_id = 0) = 0;

  /// Free the external HARQ buffer context repository entry for the associated operation.
  /// \param[in] absolute_cb_id Unique ID of associated to the CB in the soft-buffer and HARQ ACK context entry.
  virtual void free_harq_context_entry(unsigned absolute_cb_id) = 0;

  /// Checks if the hardware-accelerated PUSCH decoder uses an external HARQ buffer.
  /// \return True if an external HARQ buffer is used, false otherwise.
  virtual bool is_external_harq_supported() const = 0;
};

} // namespace hal
} // namespace srsran
