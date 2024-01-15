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

#include "srsran/phy/upper/channel_processors/pdcch_encoder.h"
#include "srsran/phy/upper/channel_processors/pdcch_modulator.h"
#include "srsran/phy/upper/channel_processors/pdcch_processor.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdcch_processor.h"

namespace srsran {

/// Describes the PDCCH processor generic implementation constructor configuration.
struct pdcch_processor_config_t {
  /// Provides the PDCCH encoder. Ownership is transferred to the PDCCH processor.
  std::unique_ptr<pdcch_encoder> encoder;
  /// Provides the PDCCH modulator. Ownership is transferred to the PDCCH processor.
  std::unique_ptr<pdcch_modulator> modulator;
  /// Provides the DMRS for PDCCH. Ownership is transferred to the PDCCH processor.
  std::unique_ptr<dmrs_pdcch_processor> dmrs;
};

/// Implements a parameter validator for \ref pdcch_processor_impl.
class pdcch_processor_validator_impl : public pdcch_pdu_validator
{
public:
  // See interface for documentation.
  bool is_valid(const pdcch_processor::pdu_t& pdu) const override { return true; }
};

/// Describes a generic PDCCH processor.
class pdcch_processor_impl : public pdcch_processor
{
private:
  /// Provides the PDCCH encoder.
  std::unique_ptr<pdcch_encoder> encoder;
  /// Provides the PDCCH modulator.
  std::unique_ptr<pdcch_modulator> modulator;
  /// Provides the DMRS for PDCCH.
  std::unique_ptr<dmrs_pdcch_processor> dmrs;
  /// Temporary encoded bits.
  std::array<uint8_t, pdcch_constants::MAX_NOF_BITS> temp_encoded;

  /// \brief Calculates the number of encoded bits for a PDCCH transmission.
  /// \param[in] aggregation_level Indicates the PDCCH transmission aggregation level.
  /// \return The number of encoded bits.
  static constexpr unsigned nof_encoded_bits(unsigned aggregation_level)
  {
    return aggregation_level * pdcch_constants::NOF_REG_PER_CCE * pdcch_constants::NOF_RE_PDCCH_PER_RB * 2;
  }

  /// \brief Computes the allocation mask as per TS38.211 Section 7.3.2.2.
  /// \param[in] coreset Provides CORESET parameters.
  /// \param[in] dci Provides DCI parameters.
  /// \return The PRB allocation mask for the PDCCH transmission.
  static bounded_bitset<MAX_RB> compute_rb_mask(const coreset_description& coreset, const dci_description& dci);

public:
  /// Generic PDSCH modulator instance constructor.
  pdcch_processor_impl(std::unique_ptr<pdcch_encoder>        encoder_,
                       std::unique_ptr<pdcch_modulator>      modulator_,
                       std::unique_ptr<dmrs_pdcch_processor> dmrs_) :
    encoder(std::move(encoder_)), modulator(std::move(modulator_)), dmrs(std::move(dmrs_))
  {
    srsran_assert(encoder, "Invalid encoder.");
    srsran_assert(modulator, "Invalid modulator.");
    srsran_assert(dmrs, "Invalid DMRS generator.");
  }

  // See interface for documentation.
  void process(resource_grid_mapper& grid, const pdu_t& pdu) override;
};

} // namespace srsran
