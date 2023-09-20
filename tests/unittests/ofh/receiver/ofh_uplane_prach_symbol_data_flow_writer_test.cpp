/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../../../../lib/ofh/receiver/ofh_uplane_prach_symbol_data_flow_writer.h"
#include "../ofh_uplane_rx_symbol_notifier_test_doubles.h"
#include "helpers.h"
#include "srsran/ofh/serdes/ofh_message_decoder_properties.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;
using namespace ofh::testing;

class ofh_uplane_prach_symbol_data_flow_writer_fixture : public ::testing::TestWithParam<prach_format_type>
{
protected:
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxc = {4, 5, 6, 7};
  const slot_point                                      slot;
  prach_format_type                                     format = GetParam();
  prach_buffer_context                                  buffer_context;
  prach_buffer_dummy                                    buffer;
  unsigned                                              preamble_length;
  unsigned                                              nof_symbols;
  uplane_message_decoder_results                        results;
  std::shared_ptr<prach_context_repository>             repo = std::make_shared<prach_context_repository>(1);
  uplane_prach_symbol_data_flow_writer                  writer;

public:
  ofh_uplane_prach_symbol_data_flow_writer_fixture() :
    slot(0, 0, 1),
    buffer(get_preamble_duration(format), is_long_preamble(format)),
    preamble_length(is_long_preamble(format) ? 839 : 139),
    nof_symbols(get_preamble_duration(format) == 0 ? 1U : get_preamble_duration(format)),
    writer(prach_eaxc, srslog::fetch_basic_logger("TEST"), repo)
  {
    buffer_context.slot             = slot;
    buffer_context.format           = format;
    buffer_context.port             = 0;
    buffer_context.nof_td_occasions = 1;
    buffer_context.nof_fd_occasions = 1;
    buffer_context.pusch_scs        = srsran::subcarrier_spacing::kHz30;

    repo->add(buffer_context, buffer);

    results.params.slot      = slot;
    results.params.symbol_id = 0;
    auto& section            = results.sections.emplace_back();
    section.iq_samples.resize(MAX_NOF_PRBS * NRE);
  }
};

INSTANTIATE_TEST_SUITE_P(prach_format,
                         ofh_uplane_prach_symbol_data_flow_writer_fixture,
                         ::testing::Values(prach_format_type::zero, prach_format_type::B4));

TEST_P(ofh_uplane_prach_symbol_data_flow_writer_fixture, empty_context_does_not_write)
{
  repo->clear(slot);

  writer.write_to_prach_buffer(prach_eaxc[0], results);

  ASSERT_TRUE(repo->get(slot).empty());
}

TEST_P(ofh_uplane_prach_symbol_data_flow_writer_fixture, no_eaxc_found_does_not_write)
{
  unsigned eaxc = 0;

  prach_context context = repo->get(slot);

  writer.write_to_prach_buffer(eaxc, results);

  ASSERT_FALSE(context.empty());
  for (unsigned i = 0; i != nof_symbols; ++i) {
    const auto& sym_data = context.get_symbol_re_written(i);
    ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [](const auto& port) { return port.none(); }));
  }
}

TEST_P(ofh_uplane_prach_symbol_data_flow_writer_fixture, decoded_prbs_outside_prach_prbs_do_not_write)
{
  auto& section     = results.sections.back();
  section.nof_prbs  = 50;
  section.start_prb = 100;

  writer.write_to_prach_buffer(prach_eaxc[0], results);

  prach_context context = repo->get(slot);
  ASSERT_FALSE(context.empty());

  for (unsigned i = 0; i != nof_symbols; ++i) {
    const auto& sym_data = context.get_symbol_re_written(i);
    ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [](const auto& port) { return port.none(); }));
  }
}

TEST_P(ofh_uplane_prach_symbol_data_flow_writer_fixture, decoded_prbs_before_prach_prbs_do_not_write)
{
  buffer_context.pusch_scs = subcarrier_spacing::kHz60;
  buffer_context.format    = prach_format_type::zero;
  unsigned nof_symbols_    = 1U;
  repo->add(buffer_context, buffer);

  auto& section     = results.sections.back();
  section.nof_prbs  = 11;
  section.start_prb = 0;

  writer.write_to_prach_buffer(prach_eaxc[0], results);

  prach_context context = repo->get(slot);
  ASSERT_FALSE(context.empty());

  for (unsigned i = 0; i != nof_symbols_; ++i) {
    const auto& sym_data = context.get_symbol_re_written(i);
    ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [](const auto& port) { return port.none(); }));
  }
}

TEST_P(ofh_uplane_prach_symbol_data_flow_writer_fixture, prbs_at_the_beginning_write_the_expected_re)
{
  buffer_context.pusch_scs = subcarrier_spacing::kHz60;
  buffer_context.format    = prach_format_type::zero;
  unsigned nof_symbols_    = 1U;
  repo->add(buffer_context, buffer);

  auto& section     = results.sections.back();
  section.nof_prbs  = 1;
  section.start_prb = 11;

  writer.write_to_prach_buffer(prach_eaxc[0], results);

  prach_context context = repo->get(slot);
  ASSERT_FALSE(context.empty());

  for (unsigned i = 0; i != nof_symbols_; ++i) {
    const auto& sym_data = context.get_symbol_re_written(i);
    ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [](const auto& port) { return port.all(0, 10); }));
  }
}

TEST_P(ofh_uplane_prach_symbol_data_flow_writer_fixture, 60kHz_long_format_one_message)
{
  buffer_context.pusch_scs = subcarrier_spacing::kHz60;
  buffer_context.format    = prach_format_type::zero;
  preamble_length          = 839;
  nof_symbols              = 1U;
  buffer                   = prach_buffer_dummy(nof_symbols, is_long_preamble(buffer_context.format));
  repo->add(buffer_context, buffer);

  auto& section     = results.sections.back();
  section.nof_prbs  = 81;
  section.start_prb = 0;

  writer.write_to_prach_buffer(prach_eaxc[0], results);

  prach_context context = repo->get(slot);
  ASSERT_FALSE(context.empty());
  for (unsigned i = 0; i != nof_symbols; ++i) {
    const auto& sym_data = context.get_symbol_re_written(i);
    ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [](const auto& port) { return port.all(); }));
  }
}

TEST_P(ofh_uplane_prach_symbol_data_flow_writer_fixture, 60kHz_long_format_one_message_all_prbs)
{
  buffer_context.pusch_scs = subcarrier_spacing::kHz60;
  buffer_context.format    = prach_format_type::zero;
  preamble_length          = 839;
  nof_symbols              = 1U;
  buffer                   = prach_buffer_dummy(nof_symbols, is_long_preamble(buffer_context.format));
  repo->add(buffer_context, buffer);

  auto& section     = results.sections.back();
  section.nof_prbs  = 96;
  section.start_prb = 0;

  writer.write_to_prach_buffer(prach_eaxc[0], results);

  prach_context context = repo->get(slot);
  ASSERT_FALSE(context.empty());

  for (unsigned i = 0; i != nof_symbols; ++i) {
    const auto& sym_data = context.get_symbol_re_written(i);
    ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [](const auto& port) { return port.all(); }));
  }
}

TEST_P(ofh_uplane_prach_symbol_data_flow_writer_fixture, decoded_prbs_in_one_packet_passes)
{
  auto& section     = results.sections.back();
  section.nof_prbs  = (format == prach_format_type::zero) ? 72 : 12;
  section.start_prb = 0;

  for (unsigned symbol_id = 0; symbol_id != nof_symbols; ++symbol_id) {
    results.params.symbol_id = symbol_id;
    writer.write_to_prach_buffer(prach_eaxc[0], results);
  }

  prach_context context = repo->get(slot);
  ASSERT_FALSE(context.empty());

  for (unsigned i = 0; i != nof_symbols; ++i) {
    const auto& sym_data = context.get_symbol_re_written(i);
    ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [](const auto& port) { return port.all(); }));
  }
}

TEST_P(ofh_uplane_prach_symbol_data_flow_writer_fixture, prach_in_three_message_first_message)
{
  auto           results_cp   = results;
  const unsigned nof_symbols_ = nof_symbols;

  auto& section     = results_cp.sections.back();
  section.nof_prbs  = (format == prach_format_type::zero) ? 72 / 3 : 12 / 3;
  section.start_prb = 0;

  for (unsigned symbol_id = 0, symbol_end = nof_symbols; symbol_id != symbol_end; ++symbol_id) {
    results_cp.params.symbol_id = symbol_id;
    writer.write_to_prach_buffer(prach_eaxc[0], results_cp);
  }

  prach_context context = repo->get(slot);
  ASSERT_FALSE(context.empty());

  for (unsigned i = 0; i != nof_symbols_; ++i) {
    const auto& sym_data = context.get_symbol_re_written(i);
    ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [&](const auto& port) {
      return port.all(0, (format == prach_format_type::zero) ? 286U : 45U);
    }));
  }
}

TEST_P(ofh_uplane_prach_symbol_data_flow_writer_fixture, prach_in_three_message_second_message)
{
  auto           results_cp   = results;
  const unsigned nof_symbols_ = nof_symbols;

  auto& section     = results_cp.sections.back();
  section.nof_prbs  = (format == prach_format_type::zero) ? 72 / 3 : 12 / 3;
  section.start_prb = (format == prach_format_type::zero) ? 72 / 3 : 12 / 3;

  for (unsigned symbol_id = 0, symbol_end = nof_symbols; symbol_id != symbol_end; ++symbol_id) {
    results_cp.params.symbol_id = symbol_id;
    writer.write_to_prach_buffer(prach_eaxc[0], results_cp);
  }

  prach_context context = repo->get(slot);
  ASSERT_FALSE(context.empty());

  for (unsigned i = 0; i != nof_symbols_; ++i) {
    const auto& sym_data = context.get_symbol_re_written(i);
    ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [&](const auto& port) {
      return port.all((format == prach_format_type::zero) ? 287U : 46U,
                      (format == prach_format_type::zero) ? 574U : 93U);
    }));
  }
}

TEST_P(ofh_uplane_prach_symbol_data_flow_writer_fixture, prach_in_three_message_third_message)
{
  auto           results_cp   = results;
  const unsigned nof_symbols_ = nof_symbols;

  auto& section     = results_cp.sections.back();
  section.nof_prbs  = (format == prach_format_type::zero) ? 72 / 3 : 12 / 3;
  section.start_prb = (format == prach_format_type::zero) ? 72 / 3 * 2 : 12 / 3 * 2;

  for (unsigned symbol_id = 0, symbol_end = nof_symbols; symbol_id != symbol_end; ++symbol_id) {
    results_cp.params.symbol_id = symbol_id;
    writer.write_to_prach_buffer(prach_eaxc[0], results_cp);
  }

  prach_context context = repo->get(slot);
  ASSERT_FALSE(context.empty());

  for (unsigned i = 0; i != nof_symbols_; ++i) {
    const auto& sym_data = context.get_symbol_re_written(i);
    ASSERT_TRUE(std::all_of(sym_data.begin(), sym_data.end(), [&](const auto& port) {
      return port.all((format == prach_format_type::zero) ? 575U : 94U,
                      (format == prach_format_type::zero) ? 838U : 138U);
    }));
  }
}
