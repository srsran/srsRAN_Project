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
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;
using namespace fapi;

static void test_basic_params()
{
  pci_t             pci                        = 3;
  uint8_t           num_occasions              = 4;
  prach_format_type format_type                = prach_format_type::one;
  uint8_t           index_fd_ra                = 5;
  uint8_t           prach_start_symbol         = 5;
  uint16_t          num_cs                     = 15;
  uint8_t           is_msga_prach              = 1;
  bool              has_msga_pusch_beamforming = true;

  ul_prach_pdu         pdu;
  ul_prach_pdu_builder builder(pdu);

  builder.set_basic_parameters(pci,
                               num_occasions,
                               format_type,
                               index_fd_ra,
                               prach_start_symbol,
                               num_cs,
                               is_msga_prach,
                               has_msga_pusch_beamforming);

  TESTASSERT_EQ(pci, pdu.phys_cell_id);
  TESTASSERT_EQ(format_type, pdu.prach_format);
  TESTASSERT_EQ(num_occasions, pdu.num_prach_ocas);
  TESTASSERT_EQ(index_fd_ra, pdu.index_fd_ra);
  TESTASSERT_EQ(prach_start_symbol, pdu.prach_start_symbol);
  TESTASSERT_EQ(num_cs, pdu.num_cs);
  TESTASSERT_EQ(is_msga_prach, pdu.is_msg_a_prach);
  TESTASSERT_EQ(has_msga_pusch_beamforming, pdu.has_msg_a_pusch_beamforming);
}

static void test_maintenance_v3_basic_params()
{
  uint32_t handle                 = 123456;
  uint8_t  prach_config_scope     = 12;
  uint16_t prach_res_config_index = 1023;
  uint8_t  num_fd_ra              = 23;
  uint8_t  start_preamble_index   = 32;
  uint8_t  num_preambles_indices  = 13;

  ul_prach_pdu         pdu;
  ul_prach_pdu_builder builder(pdu);

  builder.set_maintenance_v3_basic_parameters(
      handle, prach_config_scope, prach_res_config_index, num_fd_ra, start_preamble_index, num_preambles_indices);

  const auto& v3 = pdu.maintenance_v3;
  TESTASSERT_EQ(handle, v3.handle);
  TESTASSERT_EQ(prach_config_scope, v3.prach_config_scope);
  TESTASSERT_EQ(prach_res_config_index, v3.prach_res_config_index);
  TESTASSERT_EQ(num_fd_ra, v3.num_fd_ra);
  TESTASSERT_EQ(start_preamble_index, v3.start_preamble_index);
  TESTASSERT_EQ(num_preambles_indices, v3.num_preamble_indices);
}

static void test_builder_prach_ok()
{
  test_basic_params();
  test_maintenance_v3_basic_params();
}

int main()
{
  test_builder_prach_ok();
  fmt::print("PRACH PDU builder -> OK\n");
}
