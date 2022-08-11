/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/channel_processors/pdcch_encoder.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_modulator.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pdcch_processor.h"

namespace srsgnb {

/// Describes the PDCCH processor generic implementation constructor configuration.
struct pdcch_processor_config_t {
  /// Provides the PDCCH encoder. Ownership is transferred to the PDCCH processor.
  std::unique_ptr<pdcch_encoder> encoder;
  /// Provides the PDCCH modulator. Ownership is transferred to the PDCCH processor.
  std::unique_ptr<pdcch_modulator> modulator;
  /// Provides the DMRS for PDCCH. Ownership is transferred to the PDCCH processor.
  std::unique_ptr<dmrs_pdcch_processor> dmrs;
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
    srsgnb_assert(encoder, "Invalid encoder.");
    srsgnb_assert(modulator, "Invalid modulator.");
    srsgnb_assert(dmrs, "Invalid DMRS generator.");
  }

  // See interface for documentation.
  void process(resource_grid_writer& grid, const pdu_t& pdu) override;
};

/// Creates a generic PDCCH processor.
std::unique_ptr<pdcch_processor> create_pdcch_processor(pdcch_processor_config_t& config);

} // namespace srsgnb
