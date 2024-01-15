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
/// \brief Polar code declaration.

#pragma once

#include "srsran/phy/upper/channel_coding/polar/polar_code.h"

namespace srsran {

/// Implementation of the polar code interface based on look-up tables.
class polar_code_impl : public polar_code
{
private:
  /// Look-up table for the mother code with codeblock length 32.
  static const std::array<uint16_t, 32> mother_code_5;

  /// Look-up table for the mother code with codeblock length 64.
  static const std::array<uint16_t, 64> mother_code_6;

  /// Look-up table for the mother code with codeblock length 128.
  static const std::array<uint16_t, 128> mother_code_7;

  /// Look-up table for the mother code with codeblock length 256.
  static const std::array<uint16_t, 256> mother_code_8;

  /// Look-up table for the mother code with codeblock length 512.
  static const std::array<uint16_t, 512> mother_code_9;

  /// Look-up table for the mother code with codeblock length 1024.
  static const std::array<uint16_t, 1024> mother_code_10;

  /// Look-up table for the block interleaver for codeblock length 32.
  static const std::array<uint16_t, 32> blk_interleaver_5;

  /// Look-up table for the block interleaver for codeblock length 64.
  static const std::array<uint16_t, 64> blk_interleaver_6;

  /// Look-up table for the block interleaver for codeblock length 128.
  static const std::array<uint16_t, 128> blk_interleaver_7;

  /// Look-up table for the block interleaver for codeblock length 256.
  static const std::array<uint16_t, 256> blk_interleaver_8;

  /// Look-up table for the block interleaver for codeblock length 512.
  static const std::array<uint16_t, 512> blk_interleaver_9;

  /// Look-up table for the block interleaver for codeblock length 1024
  static const std::array<uint16_t, 1024> blk_interleaver_10;

  /// Returns a view of the desired mother code (\c n is the base-2 logarithm of the codeblock length).
  static span<const uint16_t> get_mother_code(uint8_t n);

  /// Returns a view of the desired block interleaver (\c n is the base-2 logarithm of the codeblock length).
  static span<const uint16_t> get_blk_interleaver(uint8_t n);

  /// \brief Extracts the elements in \c x that are smaller than \c T or are in \c y.
  /// \return The length of the output vector \c z.
  static unsigned
  setdiff_stable(const uint16_t* x, const uint16_t* y, uint16_t* z, uint16_t T, uint16_t len1, uint16_t len2);

  /// Computes all the internal data fields based on the message size \c K, the rate-matched codeword size \c E and the
  /// base-2 logarithm of the maximum codeblock length \c nMax.
  void set_code_params(unsigned K, unsigned E, uint8_t nMax);

  /// Number of coded bits.
  unsigned N = 0;
  /// Base-2 logarithm of the number of coded bits.
  uint8_t n = 0;
  /// Number of message bits (data and CRC).
  unsigned K = 0;
  /// Number of rate-matched encoded bits.
  unsigned E = 0;
  /// Number of parity-check bits.
  unsigned nPC = 0;
  /// Number of parity-check bits of minimum bandwidth type.
  unsigned nWmPC = 0;
  /// Set of indices corresponding to the positions occupied by message bits at the encoder input.
  bounded_bitset<NMAX> K_set_mask;
  /// Set of indices corresponding to the positions occupied by parity-check bits at the encoder input.
  std::array<uint16_t, 4> PC_set;
  /// Set of indices corresponding to the positions occupied by frozen bits at the encoder input.
  bounded_bitset<NMAX> F_set_mask;
  /// Status of the internal bit interleaver.
  polar_code_ibil ibil;
  /// Temporal storage of indices corresponding to the positions occupied by message bits at the encoder input.
  std::array<uint16_t, NMAX> tmp_K_set;
  /// Temporal storage of indices corresponding to the positions occupied by frozen bits at the encoder input.
  std::array<uint16_t, NMAX> tmp_F_set;

public:
  /// \brief Default constructor.
  /// \warning The default configuration resulting from the constructor is invalid. The user must use the \ref set()
  /// method to provide a valid code configuration.
  polar_code_impl();

  /// Default destructor.
  ~polar_code_impl() = default;

  // See interface for the documentation of public methods.
  unsigned                    get_n() const override;
  unsigned                    get_N() const override;
  unsigned                    get_K() const override;
  unsigned                    get_E() const override;
  unsigned                    get_nPC() const override;
  const bounded_bitset<NMAX>& get_K_set() const override;
  span<const uint16_t>        get_PC_set() const override;
  const bounded_bitset<NMAX>& get_F_set() const override;
  span<const uint16_t>        get_blk_interleaver() const override;
  polar_code_ibil             get_ibil() const override;

  void set(unsigned K, unsigned E, uint8_t nMax, polar_code_ibil ibil) override;
};

} // namespace srsran
