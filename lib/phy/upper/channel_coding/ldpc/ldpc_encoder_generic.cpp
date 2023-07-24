/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ldpc_encoder_generic.h"
#include "srsran/srsvec/binary.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;
using namespace srsran::ldpc;

void ldpc_encoder_generic::select_strategy()
{
  ldpc_base_graph_type current_bg       = current_graph->get_base_graph();
  uint8_t              current_ls_index = current_graph->get_lifting_index();

  if (current_bg == ldpc_base_graph_type::BG1) {
    if (current_ls_index == 6) {
      high_rate = &ldpc_encoder_generic::high_rate_bg1_i6;
      return;
    }
    // If lifting index is not 6.
    high_rate = &ldpc_encoder_generic::high_rate_bg1_other;
    return;
  }
  if (current_bg == ldpc_base_graph_type::BG2) {
    if ((current_ls_index == 3) || (current_ls_index == 7)) {
      high_rate = &ldpc_encoder_generic::high_rate_bg2_i3_7;
      return;
    }
    // If lifting index is neither 3 nor 7.
    high_rate = &ldpc_encoder_generic::high_rate_bg2_other;
    return;
  }
  assert(false);
}

void ldpc_encoder_generic::preprocess_systematic_bits()
{
  for (auto& row : auxiliary) {
    std::fill(row.begin(), row.end(), 0);
  }

  // LDPC codes are systematic: the first bits of the codeblock coincide with the message.
  srsvec::copy(span<uint8_t>(codeblock).first(message.size()), message);
  // Initialize the rest to zero.
  srsvec::zero(span<uint8_t>(codeblock.data() + message.size(), codeblock_length - message.size()));

  const auto& parity_check_sparse = current_graph->get_parity_check_sparse();

  for (const auto& element : parity_check_sparse) {
    unsigned m          = std::get<0>(element);
    unsigned k          = std::get<1>(element);
    unsigned node_shift = std::get<2>(element);

    span<const uint8_t> message_chunk = message.subspan(static_cast<size_t>(k * lifting_size), lifting_size);

    // Rotate node. Equivalent to:
    // for (uint16_t l = 0; l != lifting_size; ++l) {
    //   uint16_t shifted_index = (node_shift + l) % lifting_size;
    //   auxiliary[m][l] ^= message_chunk[shifted_index];
    //   auxiliary[m][l] &= 1U;
    // }
    auto set_auxiliary_chunk = [this, m]() {
      if (m < bg_hr_parity_nodes) {
        return span<uint8_t>(auxiliary[m].data(), lifting_size);
      }
      unsigned offset = (bg_K + m) * lifting_size;
      return span<uint8_t>(codeblock.data() + offset, lifting_size);
    };
    span<uint8_t> auxiliary_chunk = set_auxiliary_chunk();

    srsvec::binary_xor(auxiliary_chunk.first(lifting_size - node_shift),
                       message_chunk.last(lifting_size - node_shift),
                       auxiliary_chunk.first(lifting_size - node_shift));
    srsvec::binary_xor(
        auxiliary_chunk.last(node_shift), message_chunk.first(node_shift), auxiliary_chunk.last(node_shift));
    std::for_each(auxiliary_chunk.begin(), auxiliary_chunk.end(), [](uint8_t& v) { v &= 1U; });
  }
}

void ldpc_encoder_generic::encode_ext_region()
{
  // We only compute the variable nodes needed to fill the codeword.
  // Also, recall the high-rate region has length (bg_K + 4) * lifting_size.
  unsigned nof_layers = codeblock_length / lifting_size - bg_K;
  for (unsigned m = bg_hr_parity_nodes; m < nof_layers; ++m) {
    unsigned skip = (bg_K + m) * lifting_size;
    for (unsigned i = 0; i != lifting_size; ++i) {
      for (unsigned k = 0; k != bg_hr_parity_nodes; ++k) {
        uint16_t node_shift = current_graph->get_lifted_node(m, bg_K + k);
        if (node_shift == NO_EDGE) {
          continue;
        }
        unsigned current_index = (bg_K + k) * lifting_size + ((i + node_shift) % lifting_size);
        codeblock[skip + i] ^= codeblock[current_index];
      }
    }
  }
}

void ldpc_encoder_generic::write_codeblock(span<uint8_t> out)
{
  // The encoder shortens the codeblock by discarding the first 2 * LS bits.
  uint8_t* first = &codeblock[2UL * lifting_size];

  srsvec::copy(out, span<uint8_t>{first, out.size()});
}

void ldpc_encoder_generic::high_rate_bg1_i6()
{
  uint16_t                                                               ls  = lifting_size;
  std::array<std::array<uint8_t, MAX_LIFTING_SIZE>, bg_hr_parity_nodes>& aux = auxiliary;

  unsigned skip0 = bg_K * ls;
  unsigned skip1 = (bg_K + 1) * ls;
  unsigned skip2 = (bg_K + 2) * ls;
  unsigned skip3 = (bg_K + 3) * ls;
  for (uint16_t k = 0; k != ls; ++k) {
    int i = (k - 105) % ls;
    i     = (i >= 0) ? i : i + ls;

    // first chunk of parity bits
    codeblock[skip0 + k] = aux[0][i] ^ aux[1][i];
    codeblock[skip0 + k] ^= aux[2][i];
    codeblock[skip0 + k] ^= aux[3][i];
    // second chunk of parity bits
    codeblock[skip1 + k] = aux[0][k] ^ codeblock[skip0 + k];
    // fourth chunk of parity bits
    codeblock[skip3 + k] = aux[3][k] ^ codeblock[skip0 + k];
    // third chunk of parity bits
    codeblock[skip2 + k] = aux[2][k] ^ codeblock[skip3 + k];
  }
}

void ldpc_encoder_generic::high_rate_bg1_other()
{
  uint16_t                                                               ls  = lifting_size;
  std::array<std::array<uint8_t, MAX_LIFTING_SIZE>, bg_hr_parity_nodes>& aux = auxiliary;

  unsigned skip0 = bg_K * ls;
  unsigned skip1 = (bg_K + 1) * ls;
  unsigned skip2 = (bg_K + 2) * ls;
  unsigned skip3 = (bg_K + 3) * ls;
  for (uint16_t k = 0; k != ls; ++k) {
    // first chunk of parity bits
    codeblock[skip0 + k] = aux[0][k] ^ aux[1][k];
    codeblock[skip0 + k] ^= aux[2][k];
    codeblock[skip0 + k] ^= aux[3][k];
  }
  for (uint16_t k = 0; k != ls; ++k) {
    // second chunk of parity bits
    codeblock[skip1 + k] = aux[0][k] ^ codeblock[skip0 + ((k + 1) % ls)];
    // fourth chunk of parity bits
    codeblock[skip3 + k] = aux[3][k] ^ codeblock[skip0 + ((k + 1) % ls)];
    // third chunk of parity bits
    codeblock[skip2 + k] = aux[2][k] ^ codeblock[skip3 + k];
  }
}

void ldpc_encoder_generic::high_rate_bg2_i3_7()
{
  uint16_t                                                               ls  = lifting_size;
  std::array<std::array<uint8_t, MAX_LIFTING_SIZE>, bg_hr_parity_nodes>& aux = auxiliary;

  unsigned skip0 = bg_K * ls;
  unsigned skip1 = (bg_K + 1) * ls;
  unsigned skip2 = (bg_K + 2) * ls;
  unsigned skip3 = (bg_K + 3) * ls;
  for (uint16_t k = 0; k != ls; ++k) {
    // first chunk of parity bits
    codeblock[skip0 + k] = aux[0][k] ^ aux[1][k];
    codeblock[skip0 + k] ^= aux[2][k];
    codeblock[skip0 + k] ^= aux[3][k];
  }
  for (uint16_t k = 0; k != ls; ++k) {
    // second chunk of parity bits
    codeblock[skip1 + k] = aux[0][k] ^ codeblock[skip0 + ((k + 1) % ls)];
    // third chunk of parity bits
    codeblock[skip2 + k] = aux[1][k] ^ codeblock[skip1 + k];
    // fourth chunk of parity bits
    codeblock[skip3 + k] = aux[3][k] ^ codeblock[skip0 + ((k + 1) % ls)];
  }
}

void ldpc_encoder_generic::high_rate_bg2_other()
{
  uint16_t                                                               ls  = lifting_size;
  std::array<std::array<uint8_t, MAX_LIFTING_SIZE>, bg_hr_parity_nodes>& aux = auxiliary;

  unsigned skip0 = bg_K * ls;
  unsigned skip1 = (bg_K + 1) * ls;
  unsigned skip2 = (bg_K + 2) * ls;
  unsigned skip3 = (bg_K + 3) * ls;

  for (uint16_t k = 0; k != ls; ++k) {
    int i = (k - 1) % ls;
    i     = (i >= 0) ? i : i + ls;

    // first chunk of parity bits
    codeblock[skip0 + k] = aux[0][i] ^ aux[1][i];
    codeblock[skip0 + k] ^= aux[2][i];
    codeblock[skip0 + k] ^= aux[3][i];

    // second chunk of parity bits
    codeblock[skip1 + k] = aux[0][k] ^ codeblock[skip0 + k];
    // third chunk of parity bits
    codeblock[skip2 + k] = aux[1][k] ^ codeblock[skip1 + k];
    // fourth chunk of parity bits
    codeblock[skip3 + k] = aux[3][k] ^ codeblock[skip0 + k];
  }
}
