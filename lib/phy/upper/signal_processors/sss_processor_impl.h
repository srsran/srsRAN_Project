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

#include "srsran/adt/complex.h"
#include "srsran/phy/upper/sequence_generators/sss_sequence_generator.h"
#include "srsran/phy/upper/signal_processors/sss_processor.h"

namespace srsran {
class sss_processor_impl : public sss_processor
{
private:
  /// First subcarrier in the SS/PBCH block.
  static const unsigned ssb_k_begin = 56;
  /// Symbol index in the SSB where the SSS is mapped.
  static const unsigned ssb_l = 2;
  /// SSS Sequence length in the SSB.
  static const unsigned sequence_len = sss_sequence_generator::sequence_length;

  /// Internal SSS sequence generator.
  static const sss_sequence_generator sequence_generator;

  /// \brief Implements TS 38.211 section 7.4.2.3.2 Mapping to physical resources
  /// \param [in] sequence provides the source of the sequence
  /// \param [out] grid provides the destination of the sequence mapping
  /// \param [in] config provides the necessary parameters to generate the sequence
  void mapping(span<const cf_t> sequence, resource_grid_writer& grid, const config_t& config) const;

public:
  void map(resource_grid_writer& grid, const config_t& config) override;
};

} // namespace srsran
