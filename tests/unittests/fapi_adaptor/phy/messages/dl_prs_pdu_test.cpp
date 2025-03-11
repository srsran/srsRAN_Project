/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "message_builder_helpers.h"
#include "srsran/fapi_adaptor/phy/messages/prs.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator_configuration.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi_adaptor;
using namespace unittest;

TEST(fapi_phy_dl_prs_pdu_adaptor, valid_pdu_pass)
{
  fapi::dl_prs_pdu                   fapi_pdu = build_valid_dl_prs_pdu();
  prs_generator_configuration        prs_config;
  slot_point                         slot(fapi_pdu.scs, 0, 1);
  auto                               pm_tools = generate_precoding_matrix_tables(1, 0);
  const precoding_matrix_repository& pm_repo  = *std::get<std::unique_ptr<precoding_matrix_repository>>(pm_tools);

  convert_prs_fapi_to_phy(prs_config, fapi_pdu, slot.sfn(), slot.slot_index(), pm_repo);

  ASSERT_EQ(slot, prs_config.slot);
  ASSERT_EQ(fapi_pdu.cp, prs_config.cp);
  ASSERT_EQ(fapi_pdu.nid_prs, prs_config.n_id_prs);
  ASSERT_EQ(fapi_pdu.comb_size, prs_config.comb_size);
  ASSERT_EQ(fapi_pdu.comb_offset, prs_config.comb_offset);
  ASSERT_EQ(fapi_pdu.num_symbols, prs_config.duration);
  ASSERT_EQ(fapi_pdu.first_symbol, prs_config.start_symbol);
  ASSERT_EQ(fapi_pdu.start_rb, prs_config.prb_start);
  ASSERT_EQ(interval<uint16_t>::start_and_len(fapi_pdu.start_rb, fapi_pdu.num_rbs), prs_config.freq_alloc);
}
