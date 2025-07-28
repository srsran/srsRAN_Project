/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/phy/upper/channel_processors/pdsch/pdsch_encoder.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_modulator.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsran/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"
#include "srsran/ran/pdsch/pdsch_constants.h"

namespace srsran {

/// Implements a generic PDSCH processor.
class pdsch_processor_impl : public pdsch_processor
{
public:
  /// \brief Creates a generic PDSCH processor.
  pdsch_processor_impl(std::unique_ptr<pdsch_encoder>        encoder_,
                       std::unique_ptr<pdsch_modulator>      modulator_,
                       std::unique_ptr<dmrs_pdsch_processor> dmrs_,
                       std::unique_ptr<ptrs_pdsch_generator> ptrs_) :
    encoder(std::move(encoder_)), modulator(std::move(modulator_)), dmrs(std::move(dmrs_)), ptrs(std::move(ptrs_))
  {
    srsran_assert(encoder != nullptr, "Invalid encoder pointer.");
    srsran_assert(modulator != nullptr, "Invalid modulator pointer.");
    srsran_assert(dmrs != nullptr, "Invalid dmrs pointer.");
    srsran_assert(ptrs != nullptr, "Invalid ptrs pointer.");
  }

  // See interface for documentation.
  void process(resource_grid_writer&                                           grid,
               pdsch_processor_notifier&                                       notifier,
               static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                    pdu) override;

private:
  /// \brief Encodes a codeword as per TS38.212 Section 7.2.
  ///
  /// \param[in]     data        Provides the data to transmit as packed bits.
  /// \param[in]     codeword_id Indicates the codeword identifier.
  /// \param[in]     nof_layers  Indicates the number of layers the codeword is mapped on to.
  /// \param[in]     Nre         Indicates the number of resource elements used for PDSCH mapping.
  /// \param[in]     pdu         Provides the PDSCH processor PDU.
  /// \return A view of the encoded codeword.
  const bit_buffer&
  encode(span<const uint8_t> data, unsigned codeword_id, unsigned nof_layers, unsigned Nre, const pdu_t& pdu);

  /// \brief Modulates a PDSCH transmission as per TS38.211 Section 7.3.1.
  /// \param[out] grid          Resource grid writer interface.
  /// \param[in] temp_codewords Provides the encoded codewords.
  /// \param[in] pdu            Provides the PDSCH processor PDU.
  void modulate(resource_grid_writer& grid, span<const bit_buffer> temp_codewords, const pdu_t& pdu);

  std::unique_ptr<pdsch_encoder>                                  encoder;
  std::unique_ptr<pdsch_modulator>                                modulator;
  std::unique_ptr<dmrs_pdsch_processor>                           dmrs;
  std::unique_ptr<ptrs_pdsch_generator>                           ptrs;
  std::array<uint8_t, pdsch_constants::CODEWORD_MAX_SIZE.value()> temp_unpacked_codeword;
  std::array<static_bit_buffer<pdsch_constants::CODEWORD_MAX_SIZE.value()>, MAX_NOF_TRANSPORT_BLOCKS>
      temp_packed_codewords;
};

} // namespace srsran
