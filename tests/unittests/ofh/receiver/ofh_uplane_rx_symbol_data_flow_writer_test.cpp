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

#include "../../../../lib/ofh/receiver/ofh_uplane_rx_symbol_data_flow_writer.h"
#include "helpers.h"
#include "srsran/ofh/serdes/ofh_message_decoder_properties.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;
using namespace ofh::testing;

class ofh_uplane_rx_symbol_data_flow_writer_fixture : public ::testing::Test
{
protected:
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> eaxc = {0, 1, 2, 3};
  std::shared_ptr<uplink_context_repository>      repo =
      std::make_shared<uplink_context_repository>(1, srslog::fetch_basic_logger("TEST"));
  unsigned                            sector = 0;
  slot_point                          slot;
  unsigned                            symbol_id = 0;
  resource_grid_writer_bool_spy       rg_writer;
  resource_grid_dummy_with_spy_writer grid;
  uplane_message_decoder_results      results;
  uplane_rx_symbol_data_flow_writer   writer;

public:
  ofh_uplane_rx_symbol_data_flow_writer_fixture() :
    slot(0, 0, 1), grid(rg_writer), writer(eaxc, srslog::fetch_basic_logger("TEST"), repo)
  {
    results.params.slot      = slot;
    results.params.symbol_id = symbol_id;
    auto& section            = results.sections.emplace_back();
    section.iq_samples.resize(MAX_NOF_PRBS * NRE);
  }
};

TEST_F(ofh_uplane_rx_symbol_data_flow_writer_fixture, empty_context_does_not_write)
{
  writer.write_to_resource_grid(eaxc[0], results);

  ASSERT_TRUE(repo->get(results.params.slot, results.params.symbol_id).empty());
  ASSERT_FALSE(rg_writer.has_grid_been_written());
}

#ifdef ASSERTS_ENABLED
TEST_F(ofh_uplane_rx_symbol_data_flow_writer_fixture, death_test_no_eaxc_found)
{
  unsigned invalid_eaxc = 4;

  repo->add({results.params.slot, sector}, grid);
  ASSERT_FALSE(repo->get(results.params.slot, results.params.symbol_id).empty());

  ASSERT_DEATH(writer.write_to_resource_grid(invalid_eaxc, results), "");
}
#endif

TEST_F(ofh_uplane_rx_symbol_data_flow_writer_fixture, decoded_prbs_outside_grid_prbs_do_not_write)
{
  auto& section     = results.sections.back();
  section.nof_prbs  = 50;
  section.start_prb = 51;

  repo->add({results.params.slot, sector}, grid);
  writer.write_to_resource_grid(eaxc[0], results);

  ASSERT_FALSE(repo->get(results.params.slot, results.params.symbol_id).empty());
  ASSERT_FALSE(rg_writer.has_grid_been_written());

  uplink_context context  = repo->get(slot, symbol_id);
  const auto&    sym_data = context.get_re_written_mask();
  ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [](const auto& port) { return port.none(); }));
}

TEST_F(ofh_uplane_rx_symbol_data_flow_writer_fixture, decoded_prbs_match_grid_prbs_write)
{
  auto& section     = results.sections.back();
  section.nof_prbs  = 51;
  section.start_prb = 0;

  repo->add({results.params.slot, sector}, grid);
  writer.write_to_resource_grid(eaxc[0], results);

  ASSERT_FALSE(repo->get(results.params.slot, results.params.symbol_id).empty());
  ASSERT_TRUE(rg_writer.has_grid_been_written());
  ASSERT_EQ(section.nof_prbs, rg_writer.get_nof_prbs_written());

  uplink_context context  = repo->get(slot, symbol_id);
  const auto&    sym_data = context.get_re_written_mask();
  ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [](const auto& port) { return port.all(); }));
}

TEST_F(ofh_uplane_rx_symbol_data_flow_writer_fixture, decoded_prbs_bigger_than_grid_prbs_write)
{
  auto& section     = results.sections.back();
  section.nof_prbs  = 273;
  section.start_prb = 0;

  repo->add({results.params.slot, sector}, grid);
  writer.write_to_resource_grid(eaxc[0], results);

  ASSERT_FALSE(repo->get(results.params.slot, results.params.symbol_id).empty());
  ASSERT_TRUE(rg_writer.has_grid_been_written());
  ASSERT_EQ(rg_writer.get_nof_subc() / NRE, rg_writer.get_nof_prbs_written());

  uplink_context context  = repo->get(slot, symbol_id);
  const auto&    sym_data = context.get_re_written_mask();
  ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [](const auto& port) { return port.all(); }));
}

TEST_F(ofh_uplane_rx_symbol_data_flow_writer_fixture, segmented_prbs_inside_the_grid_write)
{
  auto& section     = results.sections.back();
  section.nof_prbs  = 10;
  section.start_prb = 0;

  repo->add({results.params.slot, sector}, grid);
  writer.write_to_resource_grid(eaxc[0], results);

  ASSERT_FALSE(repo->get(results.params.slot, results.params.symbol_id).empty());
  ASSERT_TRUE(rg_writer.has_grid_been_written());
  ASSERT_EQ(section.nof_prbs, rg_writer.get_nof_prbs_written());

  uplink_context context  = repo->get(slot, symbol_id);
  const auto&    sym_data = context.get_re_written_mask();
  ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [](const auto& port) { return port.all(0, 9 * NRE); }));
}

TEST_F(ofh_uplane_rx_symbol_data_flow_writer_fixture, segmented_prbs_write_the_prbs_overlapped_with_grid)
{
  auto& section     = results.sections.back();
  section.nof_prbs  = 60;
  section.start_prb = 40;

  repo->add({results.params.slot, sector}, grid);
  writer.write_to_resource_grid(eaxc[0], results);

  ASSERT_FALSE(repo->get(results.params.slot, results.params.symbol_id).empty());
  ASSERT_TRUE(rg_writer.has_grid_been_written());
  ASSERT_EQ(11, rg_writer.get_nof_prbs_written());

  uplink_context context  = repo->get(slot, symbol_id);
  const auto&    sym_data = context.get_re_written_mask();
  ASSERT_TRUE(
      std::all_of(sym_data.begin(), sym_data.end(), [](const auto& port) { return port.all(40 * NRE, 50 * NRE); }));
}

TEST_F(ofh_uplane_rx_symbol_data_flow_writer_fixture, segmented_prbs_fill_the_grid)
{
  auto& section     = results.sections.back();
  section.nof_prbs  = 50;
  section.start_prb = 0;

  repo->add({results.params.slot, sector}, grid);
  writer.write_to_resource_grid(eaxc[0], results);
  ASSERT_EQ(section.nof_prbs, rg_writer.get_nof_prbs_written());
  {
    uplink_context context  = repo->get(slot, symbol_id);
    const auto&    sym_data = context.get_re_written_mask();
    ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [](const auto& port) { return port.all(0, 49 * NRE); }));
  }

  unsigned nof_prbs = section.nof_prbs;

  section.nof_prbs  = 1;
  section.start_prb = 50;
  nof_prbs += section.nof_prbs;

  writer.write_to_resource_grid(eaxc[0], results);

  ASSERT_FALSE(repo->get(results.params.slot, results.params.symbol_id).empty());
  ASSERT_TRUE(rg_writer.has_grid_been_written());
  ASSERT_EQ(nof_prbs, rg_writer.get_nof_prbs_written());
  uplink_context context  = repo->get(slot, symbol_id);
  const auto&    sym_data = context.get_re_written_mask();
  ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [](const auto& port) { return port.all(); }));
}
