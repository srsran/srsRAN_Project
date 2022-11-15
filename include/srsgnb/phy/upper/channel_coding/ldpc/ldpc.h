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
/// \brief LDPC constants, utilities and graphs.
///
/// This file defines types, constants and classes to represent an LDPC Tanner graph
/// according to TS38.212 Section 5.3.2.
#pragma once

#include "srsgnb/ran/ldpc_base_graph.h"
#include "srsgnb/support/math_utils.h"
#include <array>

namespace srsgnb {

/// LDPC constants and helper functions.
namespace ldpc {

/// Available lifting sizes.
enum lifting_size_t {
  LS2   = 2,
  LS3   = 3,
  LS4   = 4,
  LS5   = 5,
  LS6   = 6,
  LS7   = 7,
  LS8   = 8,
  LS9   = 9,
  LS10  = 10,
  LS11  = 11,
  LS12  = 12,
  LS13  = 13,
  LS14  = 14,
  LS15  = 15,
  LS16  = 16,
  LS18  = 18,
  LS20  = 20,
  LS22  = 22,
  LS24  = 24,
  LS26  = 26,
  LS28  = 28,
  LS30  = 30,
  LS32  = 32,
  LS36  = 36,
  LS40  = 40,
  LS44  = 44,
  LS48  = 48,
  LS52  = 52,
  LS56  = 56,
  LS60  = 60,
  LS64  = 64,
  LS72  = 72,
  LS80  = 80,
  LS88  = 88,
  LS96  = 96,
  LS104 = 104,
  LS112 = 112,
  LS120 = 120,
  LS128 = 128,
  LS144 = 144,
  LS160 = 160,
  LS176 = 176,
  LS192 = 192,
  LS208 = 208,
  LS224 = 224,
  LS240 = 240,
  LS256 = 256,
  LS288 = 288,
  LS320 = 320,
  LS352 = 352,
  LS384 = 384
};
/// Total number of lifting sizes.
constexpr unsigned NOF_LIFTING_SIZES = 51;
/// All lifting sizes are assigned an index from 0 to 7 (see TS38.212 Table 5.3.2-1).
constexpr unsigned NOF_LIFTING_INDICES = 8;

/// Array of lifting sizes, for iterations.
static constexpr std::array<lifting_size_t, NOF_LIFTING_SIZES> all_lifting_sizes = {
    LS2,   LS3,   LS4,   LS5,   LS6,   LS7,   LS8,   LS9,   LS10,  LS11,  LS12,  LS13,  LS14,
    LS15,  LS16,  LS18,  LS20,  LS22,  LS24,  LS26,  LS28,  LS30,  LS32,  LS36,  LS40,  LS44,
    LS48,  LS52,  LS56,  LS60,  LS64,  LS72,  LS80,  LS88,  LS96,  LS104, LS112, LS120, LS128,
    LS144, LS160, LS176, LS192, LS208, LS224, LS240, LS256, LS288, LS320, LS352, LS384};

/// Filler bit identifier
static constexpr uint8_t FILLER_BIT = 254;

/// Maximum LDPC message size in bits.
static constexpr unsigned MAX_MESSAGE_SIZE = all_lifting_sizes.back() * 22;

/// Maximum LDPC encoded codeblock size in bits.
static constexpr unsigned MAX_CODEBLOCK_SIZE = all_lifting_sizes.back() * 66;

/// \brief Maximum LDPC rate-matched codeblock size in bits.
///
/// The minimum code rate (after rate matching) is \f$30 / 1024\f$, as per TS38.214 Table 5.1.3.1-3. Therefore, the
/// maximum rate-matched codeblock length can be computed as
/// \f[
/// \textup{MAX_MESSAGE_SIZE} \times \lceil 1024 / 30 \rceil = \textup{MAX_MESSAGE_SIZE} \times 35.
/// \f]
static constexpr unsigned MAX_CODEBLOCK_RM_SIZE = MAX_MESSAGE_SIZE * 35;

/// \brief Computes the transport block CRC size from the transport block size.
///
/// \param[in] tbs Transport block size as a number of bits.
/// \return The number of CRC bits appended to a transport block.
inline constexpr unsigned compute_tb_crc_size(unsigned tbs)
{
  constexpr unsigned MAX_BITS_CRC16 = 3824;
  return ((tbs <= MAX_BITS_CRC16) ? 16 : 24);
}

/// \brief Computes the number of codeblocks from a transport block size.
///
/// \param[in] tbs Transport block size as a number of bits (not including CRC).
/// \param[in] bg  Base graph.
/// \return The number of codeblocks a transport block of size \c tbs is encoded into when using using base graph \c bg.
inline constexpr unsigned compute_nof_codeblocks(unsigned tbs, ldpc_base_graph_type bg)
{
  constexpr unsigned CBLOC_CRC_LENGTH = 24;
  unsigned           tb_and_crc_bits  = tbs + compute_tb_crc_size(tbs);

  unsigned max_segment_length = (bg == ldpc_base_graph_type::BG1) ? 8448 : 3840;

  return ((tb_and_crc_bits <= max_segment_length)
              ? 1
              : divide_ceil(tb_and_crc_bits, max_segment_length - CBLOC_CRC_LENGTH));
}

/// \brief Computes the lifting size used to encode/decode a transport block.
///
/// As per TS38.212 Section 5.2.2, the lifting size depends on the size of the transport block, on the LDPC base graph
/// and on the number of segments the transport block is split into.
inline unsigned compute_lifting_size(unsigned tbs, ldpc_base_graph_type base_graph, unsigned nof_segments)
{
  unsigned nof_tb_bits_in = tbs + compute_tb_crc_size(tbs);

  unsigned ref_length = 22;
  if (base_graph == ldpc_base_graph_type::BG2) {
    if (nof_tb_bits_in > 640) {
      ref_length = 10;
    } else if (nof_tb_bits_in > 560) {
      ref_length = 9;
    } else if (nof_tb_bits_in > 192) {
      ref_length = 8;
    } else {
      ref_length = 6;
    }
  }

  unsigned total_ref_length = nof_segments * ref_length;

  // Calculate the number of bits per transport block including codeblock CRC.
  unsigned nof_tb_bits_out = nof_tb_bits_in;
  if (nof_segments > 1) {
    nof_tb_bits_out += 24 * nof_segments;
  }

  unsigned lifting_size = 0;
  for (auto ls : all_lifting_sizes) {
    if (ls * total_ref_length >= nof_tb_bits_out) {
      lifting_size = ls;
      break;
    }
  }
  assert(lifting_size != 0);

  return lifting_size;
}

/// \brief Computes the codeblock size for the given base graph and lifting size.
///
/// As per TS38.212 Section 5.2.2, the number bits to encode is fixed for a given base graph and a given lifting size.
/// The codeblock consists of information bits, CRC bits and the number of filler bits required to match the codeblock
/// size.
inline unsigned compute_codeblock_size(ldpc_base_graph_type base_graph, unsigned lifting_size)
{
  constexpr unsigned base_length_BG1 = 22;
  constexpr unsigned base_length_BG2 = 10;
  unsigned           base_length     = (base_graph == ldpc_base_graph_type::BG1) ? base_length_BG1 : base_length_BG2;

  return base_length * lifting_size;
}

} // namespace ldpc

} // namespace srsgnb
