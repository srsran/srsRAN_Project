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

#include "srsran/ran/sch/sch_segmentation.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"

using namespace srsran;

constexpr units::bits sch_information::CB_CRC_SIZE;

sch_information srsran::get_sch_segmentation_info(units::bits tbs, float target_code_rate)
{
  srsran_assert(target_code_rate > 0.F && target_code_rate < 1.F,
                "Invalid target code rate {}, expected a value between 0 and 1",
                target_code_rate);

  sch_information result;

  // Calculate the transport block CRC size.
  result.tb_crc_size = ldpc::compute_tb_crc_size(tbs);

  // Select base graph.
  result.base_graph = get_ldpc_base_graph(target_code_rate, tbs);

  // Calculate the number of codeblocks.
  result.nof_cb = ldpc::compute_nof_codeblocks(tbs, result.base_graph);

  // Calculate the number of bits per codeblock.
  units::bits nof_payload_bits_per_cb((tbs + result.tb_crc_size).value() / result.nof_cb);
  if (result.nof_cb > 1) {
    nof_payload_bits_per_cb += sch_information::CB_CRC_SIZE;
  }

  // Calculate lifting size.
  result.lifting_size = ldpc::compute_lifting_size(tbs, result.base_graph, result.nof_cb);

  // Calculate the codeblock size.
  result.nof_bits_per_cb = ldpc::compute_codeblock_size(result.base_graph, result.lifting_size);

  // Calculate the number of filler bits per codeblock.
  result.nof_filler_bits_per_cb = result.nof_bits_per_cb - nof_payload_bits_per_cb;

  return result;
}
