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

#include "../../../../lib/ofh/receiver/ofh_data_flow_uplane_uplink_data_impl.h"
#include "../ofh_uplane_rx_symbol_notifier_test_doubles.h"
#include "helpers.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;
using namespace ofh::testing;

namespace {
class uplane_message_decoder_spy : public uplane_message_decoder
{
  uplane_message_decoder_results spy_results;

public:
  bool decode(uplane_message_decoder_results& results, span<const uint8_t> message) override
  {
    results = spy_results;

    return true;
  }

  void set_results(const uplane_message_decoder_results& results) { spy_results = results; }
};

} // namespace

class data_flow_uplane_uplink_data_impl_fixture : public ::testing::Test
{
protected:
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc = {5, 1, 2, 3};
  slot_point                                            slot;
  unsigned                                              symbol   = 0;
  unsigned                                              nof_prbs = 51;
  unsigned                                              sector   = 0;
  unsigned                                              eaxc     = 5;
  resource_grid_writer_bool_spy                         rg_writer;
  resource_grid_dummy_with_spy_writer                   grid;
  uplane_rx_symbol_notifier_spy*                        notifier;
  std::shared_ptr<uplink_cplane_context_repository>     ul_cplane_context_repo_ptr =
      std::make_shared<uplink_cplane_context_repository>(1);
  std::shared_ptr<uplink_context_repository> ul_context_repo = std::make_shared<uplink_context_repository>(1);
  uplane_message_decoder_spy*                uplane_decoder;
  data_flow_uplane_uplink_data_impl          data_flow;

public:
  data_flow_uplane_uplink_data_impl_fixture() :
    slot(0, 0, 1), grid(rg_writer), data_flow(get_config(), get_dependencies())
  {
    ul_cplane_context context;
    context.prb_start              = 0;
    context.nof_prb                = nof_prbs;
    context.nof_symbols            = 14;
    context.radio_hdr.start_symbol = 0;
    context.radio_hdr.slot         = slot;
    context.radio_hdr.filter_index = srsran::ofh::filter_index_type::standard_channel_filter;
    context.radio_hdr.direction    = data_direction::uplink;

    // Fill the contexts
    ul_cplane_context_repo_ptr->add(slot, eaxc, context);
    ul_context_repo->add({slot, sector}, grid);
  }

  data_flow_uplane_uplink_data_impl_config get_config()
  {
    data_flow_uplane_uplink_data_impl_config config;
    config.ul_eaxc = ul_eaxc;

    return config;
  }

  data_flow_uplane_uplink_data_impl_dependencies get_dependencies()
  {
    data_flow_uplane_uplink_data_impl_dependencies dependencies;

    dependencies.logger                     = &srslog::fetch_basic_logger("TEST");
    dependencies.ul_cplane_context_repo_ptr = ul_cplane_context_repo_ptr;
    dependencies.ul_context_repo            = ul_context_repo;

    {
      auto temp             = std::make_shared<uplane_rx_symbol_notifier_spy>();
      notifier              = temp.get();
      dependencies.notifier = std::move(temp);
    }
    {
      auto temp                   = std::make_unique<uplane_message_decoder_spy>();
      uplane_decoder              = temp.get();
      dependencies.uplane_decoder = std::move(temp);
    }

    return dependencies;
  }

  uplane_message_decoder_results build_valid_decoder_results()
  {
    uplane_message_decoder_results deco_results;
    deco_results.params.slot          = slot;
    deco_results.params.direction     = data_direction::uplink;
    deco_results.params.filter_index  = filter_index_type::standard_channel_filter;
    deco_results.params.symbol_id     = 0;
    auto& section                     = deco_results.sections.emplace_back();
    section.start_prb                 = 0;
    section.nof_prbs                  = nof_prbs;
    section.use_current_symbol_number = true;
    section.is_every_rb_used          = true;
    section.iq_samples.resize(MAX_NOF_PRBS * NRE);

    return deco_results;
  }
};

TEST_F(data_flow_uplane_uplink_data_impl_fixture, valid_message_containing_all_symbols_must_be_notified)
{
  uplane_message_decoder_results deco_results = build_valid_decoder_results();
  uplane_decoder->set_results(deco_results);
  data_flow.decode_type1_message(eaxc, {});

  ASSERT_TRUE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_FALSE(notifier->has_new_prach_function_been_called());
  ASSERT_EQ(nof_prbs, rg_writer.get_nof_prbs_written());
}

TEST_F(data_flow_uplane_uplink_data_impl_fixture, invalid_filter_index_does_not_write_grid)
{
  uplane_message_decoder_results deco_results = build_valid_decoder_results();
  deco_results.params.filter_index            = srsran::ofh::filter_index_type::ul_prach_preamble_1p25khz;
  uplane_decoder->set_results(deco_results);
  data_flow.decode_type1_message(eaxc, {});

  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_FALSE(notifier->has_new_prach_function_been_called());
  ASSERT_FALSE(rg_writer.has_grid_been_written());
}

TEST_F(data_flow_uplane_uplink_data_impl_fixture, invalid_prb_range_does_not_write_grid)
{
  uplane_message_decoder_results deco_results = build_valid_decoder_results();
  deco_results.sections.front().start_prb     = 51;
  uplane_decoder->set_results(deco_results);
  data_flow.decode_type1_message(eaxc, {});

  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_FALSE(notifier->has_new_prach_function_been_called());
  ASSERT_FALSE(rg_writer.has_grid_been_written());
}

TEST_F(data_flow_uplane_uplink_data_impl_fixture, partial_prb_falling_outside_cplane_prbs_does_not_write_grid)
{
  uplane_message_decoder_results deco_results = build_valid_decoder_results();
  deco_results.sections.front().start_prb     = 41;
  deco_results.sections.front().nof_prbs      = 51;
  uplane_decoder->set_results(deco_results);
  data_flow.decode_type1_message(eaxc, {});

  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_FALSE(notifier->has_new_prach_function_been_called());
  ASSERT_FALSE(rg_writer.has_grid_been_written());
}

TEST_F(data_flow_uplane_uplink_data_impl_fixture, partial_prb_inside_cplane_prbs_write_grid)
{
  uplane_message_decoder_results deco_results = build_valid_decoder_results();
  unsigned                       num_prbs     = 23;
  deco_results.sections.front().start_prb     = 10;
  deco_results.sections.front().nof_prbs      = num_prbs;
  uplane_decoder->set_results(deco_results);
  data_flow.decode_type1_message(eaxc, {});

  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_FALSE(notifier->has_new_prach_function_been_called());
  ASSERT_TRUE(rg_writer.has_grid_been_written());
  ASSERT_EQ(num_prbs, rg_writer.get_nof_prbs_written());
}
