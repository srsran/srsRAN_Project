/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_IMPL_H

#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
#include <map>

namespace srsgnb {
/// Modulation mapper based on look-up tables.
class modulation_mapper_impl : public modulation_mapper
{
private:
  /// Defines a base structure to store the modulation tables.
  struct modulator_table_s {
    /// Modulation order (i.e., number of bits per symbol).
    unsigned Qm;

    /// \brief Look-up table.
    ///
    /// The entry at position \c i provides the complex symbol corresponding to the binary expansion of \c i.
    std::vector<cf_t> table;
  };

  /// Modulation table for BPSK modualtion.
  struct bpsk_table_s : public modulator_table_s {
    bpsk_table_s();
  };

  /// Look-up table for QPSK modulation.
  struct qpsk_table_s : public modulator_table_s {
    qpsk_table_s();
  };

  /// Look-up table for 16QAM modulation.
  struct qam16_table_s : public modulator_table_s {
    qam16_table_s();
  };

  /// Look-up table for 64QAM modulation.
  struct qam64_table_s : public modulator_table_s {
    qam64_table_s();
  };

  /// Look-up table for 256QAM modulation.
  struct qam256_table_s : public modulator_table_s {
    qam256_table_s();
  };

  /// Map associating modulation schemes to the corresponding look-up tables.
  static const std::map<modulation_scheme, modulator_table_s> modulation_tables;

public:
  // See interface for the documentation.
  void modulate(span<const uint8_t> input, span<cf_t> symbols, modulation_scheme scheme) override;
};

} // namespace srsgnb
#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_IMPL_H
