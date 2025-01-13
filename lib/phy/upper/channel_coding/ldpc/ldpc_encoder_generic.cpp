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

#include "ldpc_encoder_generic.h"
#include "srsran/srsvec/binary.h"
#include "srsran/srsvec/circ_shift.h"
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
  srsran_assert(false, "Unreachable code");
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

  for (unsigned m = 0; m != bg_hr_parity_nodes; ++m) {
    const ldpc::BG_adjacency_row_t& row = current_graph->get_adjacency_row(m);

    for (uint16_t k : row) {
      if ((k >= bg_K) || (k == NO_EDGE)) {
        break;
      }

      unsigned node_shift = current_graph->get_lifted_node(m, k);
      if (node_shift == NO_EDGE) {
        break;
      }

      span<const uint8_t> message_chunk = message.subspan(static_cast<size_t>(k * lifting_size), lifting_size);

      // Rotate node. Equivalent to:
      // for (uint16_t l = 0; l != lifting_size; ++l) {
      //   uint16_t shifted_index = (node_shift + l) % lifting_size;
      //   auxiliary[m][l] ^= message_chunk[shifted_index];
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
    }
  }
}

void ldpc_encoder_generic::ext_region_inner(span<uint8_t> out_node, unsigned m) const
{
  span<const uint8_t> codeblock_view(codeblock);

  // Get the adjacency row for the node m.
  const ldpc::BG_adjacency_row_t& row = current_graph->get_adjacency_row(m);

  // Zero node.
  srsvec::zero(out_node);

  // Process each of the nodes.
  for (uint16_t k : row) {
    // Check if the node is in codeblock range.
    if ((k >= bg_K) || (k == NO_EDGE)) {
      break;
    }

    // Obtain the node cyclic shift.
    unsigned node_shift = current_graph->get_lifted_node(m, k);
    if (node_shift == NO_EDGE) {
      break;
    }

    // Get the view of the codeblock node.
    span<const uint8_t> codeblock_node = codeblock_view.subspan(k * lifting_size, lifting_size);

    // The first time, the node is copied; afterwards combined.
    if (k == row[0]) {
      srsvec::circ_shift_backward(out_node, codeblock_node, node_shift);
    } else {
      srsvec::binary_xor(out_node.first(lifting_size - node_shift),
                         codeblock_node.last(lifting_size - node_shift),
                         out_node.first(lifting_size - node_shift));
      srsvec::binary_xor(out_node.last(node_shift), codeblock_node.first(node_shift), out_node.last(node_shift));
    }
  }

  for (unsigned k = 0; k != bg_hr_parity_nodes; ++k) {
    unsigned node_shift = current_graph->get_lifted_node(m, bg_K + k);

    if (node_shift == NO_EDGE) {
      continue;
    }

    // At this point, the codeblock nodes in the extended region already contain the contribution from the systematic
    // bits (they were computed in systematic_bits_inner). All is left to do is to sum the contribution due to the
    // high-rate region (also stored in codeblock), with the proper circular shifts.
    span<const uint8_t> in_node = codeblock_view.subspan((bg_K + k) * lifting_size, lifting_size);
    srsvec::binary_xor(out_node.first(lifting_size - node_shift),
                       in_node.last(lifting_size - node_shift),
                       out_node.first(lifting_size - node_shift));
    srsvec::binary_xor(out_node.last(node_shift), in_node.first(node_shift), out_node.last(node_shift));
  }
}

void ldpc_encoder_generic::write_codeblock(span<uint8_t> out, unsigned offset) const
{
  srsran_assert(out.size() + offset <= bg_N_short * lifting_size,
                "The output size (i.e., {}) plus the offset (i.e., {}) exceeds the codeblock length (i.e., {}).",
                out.size(),
                offset,
                bg_N_short * lifting_size);

  // Calculate the node size in bytes, considering SIMD alignment.
  span<const uint8_t> codeblock_view(codeblock);

  // Select the initial node. The first two blocks are shortened and the last node is not considered, since it can be
  // incomplete.
  unsigned i_node_begin = 2 + offset / lifting_size;

  // Advance the codeblock to the initial node.
  codeblock_view = codeblock_view.last(codeblock.size() - i_node_begin * lifting_size);

  // End node.
  unsigned i_node_end = i_node_begin + divide_ceil(out.size(), lifting_size);

  // Calculate the offset within the first node.
  offset = offset % lifting_size;

  for (unsigned i_node = i_node_begin; i_node != i_node_end; ++i_node) {
    // Determine the number of bits to read from this node.
    unsigned count = std::min(lifting_size - offset, static_cast<unsigned>(out.size()));

    // Check if it is operating in the extended region.
    if (i_node < bg_hr_parity_nodes + bg_K) {
      srsvec::copy(out.first(count), codeblock_view.subspan(offset, count));
    } else {
      // Detect if the extended region can be done directly on the output.
      bool inplace = ((offset == 0) && (count == lifting_size));

      // Create temporary data.
      static_vector<uint8_t, MAX_LIFTING_SIZE> temp(lifting_size);
      span<uint8_t>                            node_view = temp;

      // Use direct output for the extended node.
      if (inplace) {
        node_view = out.first(lifting_size);
      }

      // Process extended region for the given node.
      ext_region_inner(node_view, i_node - bg_K);

      // Copy data if temporal buffer was used.
      if (!inplace) {
        srsvec::copy(out.first(count), node_view.subspan(offset, count));
      }
    }

    // Advance codeblock.
    codeblock_view = codeblock_view.last(codeblock_view.size() - lifting_size);

    // Advance output.
    out = out.last(out.size() - count);

    // The offset is no longer required after the first node.
    offset = 0;
  }
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
