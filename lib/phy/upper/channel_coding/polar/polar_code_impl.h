/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Polar code declaration.

#pragma once

#include "srsgnb/phy/upper/channel_coding/polar/polar_code.h"
#include "srsgnb/srsvec/aligned_vec.h"

namespace srsgnb {

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
  static uint16_t
  setdiff_stable(const uint16_t* x, const uint16_t* y, uint16_t* z, int T, uint16_t len1, uint16_t len2);

  /// Computes all the internal data fields based on the message size \c K, the rate-matched codeword size \c E and the
  /// base-2 logarithm of the maximum codeblock length \c nMax.
  void set_code_params(uint16_t K, uint16_t E, uint8_t nMax);

  /// Number of coded bits.
  uint16_t N = 0;
  /// Base-2 logarithm of the number of coded bits.
  uint8_t n = 0;
  /// Number of message bits (data and CRC).
  uint16_t K = 0;
  /// Number of rate-matched encoded bits.
  uint16_t E = 0;
  /// Number of parity-check bits.
  uint16_t nPC = 0;
  /// Number of parity-check bits of minimum bandwidth type.
  uint16_t nWmPC = 0;
  /// Number of frozen bits.
  uint16_t F_set_size = 0;
  /// Set of indices corresponding to the positions occupied by message bits at the encoder input.
  bounded_bitset<NMAX> K_set_mask;
  /// Temporal set ot message bit indices.
  srsvec::aligned_vec<uint16_t> tmp_K_set;
  /// Set of indices corresponding to the positions occupied by parity-check bits at the encoder input.
  std::array<uint16_t, 4> PC_set;
  /// Set of indices corresponding to the positions occupied by frozen bits at the encoder input.
  srsvec::aligned_vec<uint16_t> F_set;
  bounded_bitset<NMAX>          F_set_mask;
  /// Status of the internal bit interleaver.
  polar_code_ibil ibil;

public:
  /// \brief Default constructor.
  /// \warning The default configuration resulting from the constructor is invalid. The user must use the \ref set()
  /// method to provide a valid code configuration.
  polar_code_impl();

  /// Default destructor.
  ~polar_code_impl() = default;

  // See interface for the documentation of public methods.
  uint16_t                    get_n() const override;
  uint16_t                    get_N() const override;
  uint16_t                    get_K() const override;
  uint16_t                    get_E() const override;
  uint16_t                    get_nPC() const override;
  const bounded_bitset<NMAX>& get_K_set() const override;
  span<const uint16_t>        get_PC_set() const override;
  const bounded_bitset<NMAX>& get_F_set() const override;
  span<const uint16_t>        get_mother_code() const override;
  span<const uint16_t>        get_blk_interleaver() const override;
  polar_code_ibil             get_ibil() const override;

  void set(uint16_t K, uint16_t E, uint8_t nMax, polar_code_ibil ibil) override;
};

} // namespace srsgnb
