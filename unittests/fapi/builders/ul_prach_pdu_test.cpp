/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi/message_builders.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace fapi;

TEST(ul_prach_pdu_builder, valid_basic_parameters_passes)
{
  pci_t             pci                = 3;
  uint8_t           num_occasions      = 4;
  prach_format_type format_type        = prach_format_type::one;
  uint8_t           index_fd_ra        = 5;
  uint8_t           prach_start_symbol = 5;
  uint16_t          num_cs             = 15;

  ul_prach_pdu         pdu;
  ul_prach_pdu_builder builder(pdu);

  builder.set_basic_parameters(pci, num_occasions, format_type, index_fd_ra, prach_start_symbol, num_cs);

  ASSERT_EQ(pci, pdu.phys_cell_id);
  ASSERT_EQ(format_type, pdu.prach_format);
  ASSERT_EQ(num_occasions, pdu.num_prach_ocas);
  ASSERT_EQ(index_fd_ra, pdu.index_fd_ra);
  ASSERT_EQ(prach_start_symbol, pdu.prach_start_symbol);
  ASSERT_EQ(num_cs, pdu.num_cs);
  ASSERT_EQ(0, pdu.is_msg_a_prach);
  ASSERT_TRUE(!pdu.has_msg_a_pusch_beamforming);
}

TEST(ul_prach_pdu_builder, valid_maintenance_v3_basic_parameters_passes)
{
  for (unsigned i = 0, e = 2; i != e; ++i) {
    uint32_t                handle                 = 123456;
    prach_config_scope_type prach_config_scope     = prach_config_scope_type::phy_context;
    uint16_t                prach_res_config_index = 1023;
    uint8_t                 num_fd_ra              = 23;
    uint8_t                 num_preambles_indices  = 13;
    optional<uint8_t>       start_preamble_index;
    if (i) {
      start_preamble_index.emplace(12);
    }

    ul_prach_pdu         pdu;
    ul_prach_pdu_builder builder(pdu);

    builder.set_maintenance_v3_basic_parameters(
        handle, prach_config_scope, prach_res_config_index, num_fd_ra, start_preamble_index, num_preambles_indices);

    const auto& v3 = pdu.maintenance_v3;
    ASSERT_EQ(handle, v3.handle);
    ASSERT_EQ(prach_config_scope, v3.prach_config_scope);
    ASSERT_EQ(prach_res_config_index, v3.prach_res_config_index);
    ASSERT_EQ(num_fd_ra, v3.num_fd_ra);
    ASSERT_EQ(start_preamble_index ? start_preamble_index.value() : 255U, v3.start_preamble_index);
    ASSERT_EQ(num_preambles_indices, v3.num_preamble_indices);
  }
}
