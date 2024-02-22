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

#include "../../../../lib/ofh/receiver/ofh_data_flow_uplane_uplink_prach_impl.h"
#include "../ofh_uplane_rx_symbol_notifier_test_doubles.h"
#include "helpers.h"
#include "srsran/ofh/serdes/ofh_message_decoder_properties.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder.h"
#include <gtest/gtest.h>
#include <tuple>

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

class data_flow_uplane_uplink_prach_impl_fixture : public ::testing::TestWithParam<std::tuple<prach_format_type, bool>>
{
protected:
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc = {5, 1, 2, 3};
  std::tuple<prach_format_type, bool>                   params  = GetParam();
  slot_point                                            slot;
  unsigned                                              symbol            = 0;
  unsigned                                              sector            = 0;
  unsigned                                              eaxc              = 5;
  bool                                                  is_cplane_enabled = std::get<1>(params);
  prach_format_type                                     format            = std::get<0>(params);

  prach_buffer_context                              buffer_context;
  prach_buffer_dummy                                buffer;
  unsigned                                          preamble_length;
  unsigned                                          nof_symbols;
  message_decoder_results                           results;
  std::shared_ptr<prach_context_repository>         repo = std::make_shared<prach_context_repository>(1);
  uplane_rx_symbol_notifier_spy*                    notifier;
  std::shared_ptr<uplink_cplane_context_repository> ul_cplane_context_repo_ptr =
      std::make_shared<uplink_cplane_context_repository>(1);
  std::shared_ptr<prach_context_repository> prach_context_repo = std::make_shared<prach_context_repository>(1);
  uplane_message_decoder_spy*               uplane_decoder;
  data_flow_uplane_uplink_prach_impl        data_flow;

public:
  data_flow_uplane_uplink_prach_impl_fixture() :
    slot(0, 0, 1),
    buffer(get_preamble_duration(format), is_long_preamble(format)),
    preamble_length(is_long_preamble(format) ? 839 : 139),
    nof_symbols(get_preamble_duration(format) == 0 ? 1U : get_preamble_duration(format)),
    data_flow(get_config(), get_dependencies())
  {
    buffer_context.slot             = slot;
    buffer_context.format           = format;
    buffer_context.ports            = {0};
    buffer_context.nof_td_occasions = 1;
    buffer_context.nof_fd_occasions = 1;
    buffer_context.pusch_scs        = srsran::subcarrier_spacing::kHz30;
    buffer_context.start_symbol     = 0;

    repo->add(buffer_context, buffer);

    results.uplane_results.params.slot      = slot;
    results.uplane_results.params.symbol_id = 0;
    results.eaxc                            = 4;
    auto& section                           = results.uplane_results.sections.emplace_back();
    section.iq_samples.resize(MAX_NOF_PRBS * NRE);

    ul_cplane_context context;
    context.prb_start              = 0;
    context.nof_prb                = 273;
    context.nof_symbols            = nof_symbols;
    context.radio_hdr.start_symbol = 0;
    context.radio_hdr.slot         = slot;
    context.radio_hdr.filter_index = srsran::ofh::filter_index_type::ul_prach_preamble_1p25khz;
    context.radio_hdr.direction    = data_direction::uplink;

    // Fill the contexts
    ul_cplane_context_repo_ptr->add(slot, eaxc, context);
    prach_context_repo->add(buffer_context, buffer);
  }

  data_flow_uplane_uplink_prach_impl_config get_config()
  {
    data_flow_uplane_uplink_prach_impl_config config;
    config.prach_eaxcs             = ul_eaxc;
    config.is_prach_cplane_enabled = is_cplane_enabled;

    return config;
  }

  data_flow_uplane_uplink_prach_impl_dependencies get_dependencies()
  {
    data_flow_uplane_uplink_prach_impl_dependencies dependencies;

    dependencies.logger                     = &srslog::fetch_basic_logger("TEST");
    dependencies.ul_cplane_context_repo_ptr = ul_cplane_context_repo_ptr;
    dependencies.prach_context_repo         = prach_context_repo;

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
    deco_results.params.filter_index  = filter_index_type::ul_prach_preamble_1p25khz;
    deco_results.params.symbol_id     = 0;
    auto& section                     = deco_results.sections.emplace_back();
    section.start_prb                 = 0;
    section.nof_prbs                  = 273;
    section.use_current_symbol_number = true;
    section.is_every_rb_used          = true;
    section.iq_samples.resize(MAX_NOF_PRBS * NRE);

    return deco_results;
  }
};

INSTANTIATE_TEST_SUITE_P(prach_format,
                         data_flow_uplane_uplink_prach_impl_fixture,
                         ::testing::Combine(::testing::Values(prach_format_type::zero, prach_format_type::B4),
                                            ::testing::Values(true, false)));

TEST_P(data_flow_uplane_uplink_prach_impl_fixture, valid_message_containing_all_symbols_must_be_notified)
{
  for (unsigned i = 0, e = nof_symbols; i != e; ++i) {
    uplane_message_decoder_results deco_results = build_valid_decoder_results();
    deco_results.params.symbol_id               = i;
    uplane_decoder->set_results(deco_results);

    data_flow.decode_type1_message(eaxc, {});
  }

  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_TRUE(notifier->has_new_prach_function_been_called());
}

TEST_P(data_flow_uplane_uplink_prach_impl_fixture, invalid_filter_index_does_not_write_buffer)
{
  uplane_message_decoder_results deco_results = build_valid_decoder_results();
  deco_results.params.filter_index            = srsran::ofh::filter_index_type::standard_channel_filter;
  uplane_decoder->set_results(deco_results);
  data_flow.decode_type1_message(eaxc, {});

  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_FALSE(notifier->has_new_prach_function_been_called());
}

TEST_P(data_flow_uplane_uplink_prach_impl_fixture, prbs_outside_prahc_range_does_not_notify)
{
  for (unsigned i = 0, e = nof_symbols; i != e; ++i) {
    uplane_message_decoder_results deco_results = build_valid_decoder_results();
    deco_results.params.symbol_id               = i;
    deco_results.sections.front().start_prb     = 272;
    deco_results.sections.front().nof_prbs      = 1;
    uplane_decoder->set_results(deco_results);

    data_flow.decode_type1_message(eaxc, {});
  }

  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_FALSE(notifier->has_new_prach_function_been_called());
}
