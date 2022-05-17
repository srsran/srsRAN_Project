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
#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_H

#include <array>
#include <memory>

namespace srsgnb {

/// LDPC constants and helper functions.
namespace ldpc {
/// Base graph types.
enum class base_graph_t { BG1, BG2 };

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
} // namespace ldpc

} // namespace srsgnb
#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_H