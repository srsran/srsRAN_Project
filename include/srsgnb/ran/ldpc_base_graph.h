/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <cstdint>

namespace srsgnb {

/// LDPC Base graph types.
enum class ldpc_base_graph_type : uint8_t { BG1, BG2 };

/// Returns the LDPC base graph based on the given code rate and transport block size in bytes, as per TS 38.212
/// section 7.2.2.
constexpr ldpc_base_graph_type get_ldpc_base_graph(float target_code_rate, unsigned tb_size_bytes)
{
  if (tb_size_bytes <= 292U || target_code_rate <= 0.25F || (tb_size_bytes <= 3824U && target_code_rate <= 0.67F)) {
    return ldpc_base_graph_type::BG2;
  }

  return ldpc_base_graph_type::BG1;
}

} // namespace srsgnb
