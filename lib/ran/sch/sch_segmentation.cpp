/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/sch/sch_segmentation.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc.h"

using namespace srsgnb;

constexpr units::bits sch_information::CB_CRC_SIZE;

sch_information srsgnb::get_sch_segmentation_info(units::bits tbs, float target_code_rate)
{
  srsgnb_assert(target_code_rate > 0.F && target_code_rate < 1.F,
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
