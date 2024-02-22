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

#include "../../../../lib/ofh/transmitter/ofh_data_flow_cplane_scheduling_commands_impl.h"
#include "../ecpri/ecpri_packet_builder_test_doubles.h"
#include "../ethernet/vlan_ethernet_frame_builder_test_doubles.h"
#include "srsran/ofh/ethernet/ethernet_frame_pool.h"
#include "srsran/ran/resource_block.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;

namespace {

/// Control-Plane message builder spy implementation.
class cplane_message_builder_spy : public cplane_message_builder
{
  bool                            has_build_dl_ul_message_function_been_called = false;
  bool                            has_build_prach_message_function_been_called = false;
  cplane_section_type1_parameters type1_params;
  cplane_section_type3_parameters type3_params;

public:
  unsigned build_dl_ul_radio_channel_message(span<uint8_t>                          buffer,
                                             const cplane_section_type1_parameters& msg_params) override
  {
    has_build_dl_ul_message_function_been_called = true;
    type1_params                                 = msg_params;

    return 0;
  }

  unsigned build_idle_guard_period_message(span<uint8_t>                          buffer,
                                           const cplane_section_type0_parameters& msg_params) override
  {
    return 0;
  }

  unsigned build_prach_mixed_numerology_message(span<uint8_t>                          buffer,
                                                const cplane_section_type3_parameters& msg_params) override
  {
    has_build_prach_message_function_been_called = true;
    type3_params                                 = msg_params;

    return 0;
  }

  /// Returns true if the function build_dl_ul_radio_channel_message has been called, otherwise false.
  bool has_build_dl_ul_radio_channel_message_function_been_called() const
  {
    return has_build_dl_ul_message_function_been_called;
  }

  /// Returns true if the function build_prach_mixed_numerology_message has been called, otherwise false.
  bool has_build_prach_mixed_numerology_message_function_been_called() const
  {
    return has_build_prach_message_function_been_called;
  }

  /// Returns the type 1 parameters.
  const cplane_section_type1_parameters& get_type1_param() const { return type1_params; };

  /// Returns the type 3 parameters.
  const cplane_section_type3_parameters& get_type3_param() const { return type3_params; };
};

} // namespace

class data_flow_cplane_scheduling_commands_impl_fixture : public ::testing::Test
{
protected:
  unsigned                                          ru_nof_prbs     = 51;
  ether::vlan_frame_params                          vlan_params     = {{0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 2}, 4, 8896};
  ru_compression_params                             dl_compr_params = {compression_type::none, 16};
  ru_compression_params                             ul_compr_params = {compression_type::BFP, 9};
  ru_compression_params                             prach_compr_params = {compression_type::BFP, 8};
  std::shared_ptr<uplink_cplane_context_repository> ul_cplane_context_repo =
      std::make_shared<uplink_cplane_context_repository>(58);
  std::shared_ptr<ether::eth_frame_pool>    frame_pool = std::make_shared<ether::eth_frame_pool>(units::bytes(9000), 2);
  ether::testing::vlan_frame_builder_spy*   vlan_builder;
  ecpri::testing::packet_builder_spy*       ecpri_builder;
  cplane_message_builder_spy*               cplane_builder;
  data_flow_cplane_scheduling_commands_impl data_flow;

public:
  data_flow_cplane_scheduling_commands_impl_fixture() :
    data_flow(generate_data_flow_config(), generate_data_flow_dependencies())
  {
  }

private:
  data_flow_cplane_scheduling_commands_impl_config generate_data_flow_config()
  {
    data_flow_cplane_scheduling_commands_impl_config config;

    config.ru_nof_prbs        = ru_nof_prbs;
    config.vlan_params        = vlan_params;
    config.dl_compr_params    = dl_compr_params;
    config.ul_compr_params    = ul_compr_params;
    config.prach_compr_params = prach_compr_params;

    return config;
  }

  data_flow_cplane_scheduling_commands_impl_dependencies generate_data_flow_dependencies()
  {
    data_flow_cplane_scheduling_commands_impl_dependencies dependencies;

    dependencies.ul_cplane_context_repo = ul_cplane_context_repo;
    dependencies.frame_pool             = frame_pool;
    dependencies.logger                 = &srslog::fetch_basic_logger("TEST");
    {
      auto temp               = std::make_unique<cplane_message_builder_spy>();
      cplane_builder          = temp.get();
      dependencies.cp_builder = std::move(temp);
    }
    {
      auto temp                = std::make_unique<ether::testing::vlan_frame_builder_spy>();
      vlan_builder             = temp.get();
      dependencies.eth_builder = std::move(temp);
    }
    {
      auto temp                  = std::make_unique<ecpri::testing::packet_builder_spy>();
      ecpri_builder              = temp.get();
      dependencies.ecpri_builder = std::move(temp);
    }

    return dependencies;
  }
};

TEST_F(data_flow_cplane_scheduling_commands_impl_fixture,
       calling_enqueue_section_type_1_calls_ecpri_and_vlan_builders_with_correct_params)
{
  data_flow_cplane_type_1_context context;
  context.slot         = slot_point(0, 0, 1);
  context.eaxc         = 2;
  context.direction    = data_direction::downlink;
  context.filter_type  = srsran::ofh::filter_index_type::standard_channel_filter;
  context.symbol_range = {0, 14};

  data_flow.enqueue_section_type_1_message(context);

  // Assert VLAN parameters.
  const ether::vlan_frame_params& vlan = vlan_builder->get_vlan_frame_params();
  ASSERT_TRUE(vlan_builder->has_build_vlan_frame_method_been_called());
  ASSERT_EQ(vlan_params.eth_type, vlan.eth_type);
  ASSERT_EQ(vlan_params.mac_dst_address, vlan.mac_dst_address);
  ASSERT_EQ(vlan_params.mac_src_address, vlan.mac_src_address);
  ASSERT_EQ(vlan_params.tci, vlan.tci);

  // Assert eCPRI parameters.
  ASSERT_FALSE(ecpri_builder->has_build_data_packet_method_been_called());
  ASSERT_TRUE(ecpri_builder->has_build_control_packet_method_been_called());

  // Assert there is only one packet.
  span<const ecpri::realtime_control_parameters> data_params = ecpri_builder->get_control_parameters();
  ASSERT_EQ(data_params.size(), 1);
  sequence_identifier_generator generator;
  for (const auto& param : data_params) {
    ASSERT_EQ(param.seq_id >> 8U, generator.generate(context.eaxc));
    ASSERT_EQ(param.rtc_id, context.eaxc);
  }
}

TEST_F(data_flow_cplane_scheduling_commands_impl_fixture, calling_enqueue_section_type_1_downlink_data_message_succeeds)
{
  data_flow_cplane_type_1_context context;
  context.slot         = slot_point(0, 0, 1);
  context.eaxc         = 2;
  context.direction    = data_direction::downlink;
  context.filter_type  = srsran::ofh::filter_index_type::standard_channel_filter;
  context.symbol_range = {3, 6};

  data_flow.enqueue_section_type_1_message(context);

  // Assert Open Fronthaul parameters.
  ASSERT_TRUE(cplane_builder->has_build_dl_ul_radio_channel_message_function_been_called());
  ASSERT_FALSE(cplane_builder->has_build_prach_mixed_numerology_message_function_been_called());
  const cplane_section_type1_parameters& cplane_params = cplane_builder->get_type1_param();

  // As it is downlink, assert downlink compression parameters.
  ASSERT_EQ(cplane_params.compr_params.type, dl_compr_params.type);
  ASSERT_EQ(cplane_params.compr_params.data_width, dl_compr_params.data_width);

  // Check radio header.
  const cplane_radio_application_header& radio_hdr = cplane_params.radio_hdr;
  ASSERT_EQ(radio_hdr.direction, context.direction);
  ASSERT_EQ(radio_hdr.slot, context.slot);
  ASSERT_EQ(radio_hdr.filter_index, context.filter_type);
  ASSERT_EQ(radio_hdr.start_symbol, context.symbol_range.start());

  // Check common section 0/1/3/5 fields.
  const cplane_common_section_0_1_3_5_fields& common_fields = cplane_params.section_fields.common_fields;
  // Always use section identifier 0.
  ASSERT_EQ(common_fields.section_id, 0);
  // Always start at PRB 0.
  ASSERT_EQ(common_fields.prb_start, 0);
  ASSERT_EQ(common_fields.nof_prb, ru_nof_prbs);
  // Use all the RE.
  ASSERT_EQ(common_fields.re_mask, 0xfff);
  ASSERT_EQ(common_fields.nof_symbols, context.symbol_range.length());
}

TEST_F(data_flow_cplane_scheduling_commands_impl_fixture, calling_enqueue_section_type_1_uplink_data_message_succeeds)
{
  data_flow_cplane_type_1_context context;
  context.slot         = slot_point(0, 0, 1);
  context.eaxc         = 2;
  context.direction    = data_direction::uplink;
  context.filter_type  = srsran::ofh::filter_index_type::standard_channel_filter;
  context.symbol_range = {5, 8};

  data_flow.enqueue_section_type_1_message(context);

  // Assert Open Fronthaul parameters.
  ASSERT_TRUE(cplane_builder->has_build_dl_ul_radio_channel_message_function_been_called());
  ASSERT_FALSE(cplane_builder->has_build_prach_mixed_numerology_message_function_been_called());
  const cplane_section_type1_parameters& cplane_params = cplane_builder->get_type1_param();

  // Assert uplink compression parameters.
  ASSERT_EQ(cplane_params.compr_params.type, ul_compr_params.type);
  ASSERT_EQ(cplane_params.compr_params.data_width, ul_compr_params.data_width);

  // Check radio header.
  const cplane_radio_application_header& radio_hdr = cplane_params.radio_hdr;
  ASSERT_EQ(radio_hdr.direction, context.direction);
  ASSERT_EQ(radio_hdr.slot, context.slot);
  ASSERT_EQ(radio_hdr.filter_index, context.filter_type);
  // Always start in symbol 0.
  ASSERT_EQ(radio_hdr.start_symbol, context.symbol_range.start());

  // Check common section 0/1/3/5 fields.
  const cplane_common_section_0_1_3_5_fields& common_fields = cplane_params.section_fields.common_fields;
  // Always use section identifier 0.
  ASSERT_EQ(common_fields.section_id, 0);
  // Always start at PRB 0.
  ASSERT_EQ(common_fields.prb_start, 0);
  ASSERT_EQ(common_fields.nof_prb, ru_nof_prbs);
  // Use all the RE.
  ASSERT_EQ(common_fields.re_mask, 0xfff);
  ASSERT_EQ(common_fields.nof_symbols, context.symbol_range.length());
}

TEST_F(data_flow_cplane_scheduling_commands_impl_fixture,
       calling_enqueue_section_type_1_uplink_data_message_adds_context)
{
  data_flow_cplane_type_1_context context;
  context.slot         = slot_point(0, 0, 1);
  context.eaxc         = 2;
  context.direction    = data_direction::uplink;
  context.filter_type  = srsran::ofh::filter_index_type::standard_channel_filter;
  context.symbol_range = {0, 14};

  data_flow.enqueue_section_type_1_message(context);

  // Assert that an entry was created for the uplink Control-Plane message.
  auto repo_context = ul_cplane_context_repo->get(context.slot, 0, context.filter_type, context.eaxc);
  ASSERT_TRUE(repo_context.has_value());

  // Check radio header.
  const cplane_radio_application_header& radio_hdr = repo_context.value().radio_hdr;
  ASSERT_EQ(radio_hdr.direction, context.direction);
  ASSERT_EQ(radio_hdr.slot, context.slot);
  ASSERT_EQ(radio_hdr.filter_index, context.filter_type);
  // Always start in symbol 0.
  ASSERT_EQ(radio_hdr.start_symbol, 0);

  ASSERT_EQ(repo_context.value().nof_symbols, context.symbol_range.length());
  ASSERT_EQ(repo_context.value().nof_prb, ru_nof_prbs);
  ASSERT_EQ(repo_context.value().prb_start, 0);

  // Check that the context can be found for all the configured symbols.
  for (unsigned i = 0, e = context.symbol_range.length(); i != e; ++i) {
    auto up_repo_context = ul_cplane_context_repo->get(context.slot, i, context.filter_type, context.eaxc);

    ASSERT_TRUE(up_repo_context.has_value());
  }
}

TEST_F(data_flow_cplane_scheduling_commands_impl_fixture,
       calling_enqueue_section_type_3_calls_ecpri_and_vlan_builders_with_correct_params)
{
  data_flow_cplane_scheduling_prach_context context = {{0, 0, 1},
                                                       2,
                                                       filter_index_type::ul_prach_preamble_1p25khz,
                                                       0,
                                                       1,
                                                       srsran::subcarrier_spacing::kHz30,
                                                       srsran::prach_subcarrier_spacing::kHz1_25,
                                                       0,
                                                       72,
                                                       3};

  data_flow.enqueue_section_type_3_prach_message(context);

  // Assert VLAN parameters.
  const ether::vlan_frame_params& vlan = vlan_builder->get_vlan_frame_params();
  ASSERT_TRUE(vlan_builder->has_build_vlan_frame_method_been_called());
  ASSERT_EQ(vlan_params.eth_type, vlan.eth_type);
  ASSERT_EQ(vlan_params.mac_dst_address, vlan.mac_dst_address);
  ASSERT_EQ(vlan_params.mac_src_address, vlan.mac_src_address);
  ASSERT_EQ(vlan_params.tci, vlan.tci);

  // Assert eCPRI parameters.
  ASSERT_FALSE(ecpri_builder->has_build_data_packet_method_been_called());
  ASSERT_TRUE(ecpri_builder->has_build_control_packet_method_been_called());

  // Assert there is only one packet.
  span<const ecpri::realtime_control_parameters> data_params = ecpri_builder->get_control_parameters();
  ASSERT_EQ(data_params.size(), 1);
  sequence_identifier_generator generator;
  for (const auto& param : data_params) {
    ASSERT_EQ(param.seq_id >> 8U, generator.generate(context.eaxc));
    ASSERT_EQ(param.rtc_id, context.eaxc);
  }
}

TEST_F(data_flow_cplane_scheduling_commands_impl_fixture, calling_enqueue_section_type_3_prach_message_succeeds)
{
  data_flow_cplane_scheduling_prach_context context = {{0, 0, 1},
                                                       2,
                                                       filter_index_type::ul_prach_preamble_1p25khz,
                                                       0,
                                                       1,
                                                       srsran::subcarrier_spacing::kHz30,
                                                       srsran::prach_subcarrier_spacing::kHz1_25,
                                                       0,
                                                       72,
                                                       3};

  data_flow.enqueue_section_type_3_prach_message(context);

  // Assert Open Fronthaul parameters.
  ASSERT_FALSE(cplane_builder->has_build_dl_ul_radio_channel_message_function_been_called());
  ASSERT_TRUE(cplane_builder->has_build_prach_mixed_numerology_message_function_been_called());
  const cplane_section_type3_parameters& cplane_params = cplane_builder->get_type3_param();

  // Assert PRACH compression parameters.
  ASSERT_EQ(cplane_params.compr_params.type, prach_compr_params.type);
  ASSERT_EQ(cplane_params.compr_params.data_width, prach_compr_params.data_width);

  // Check radio header.
  const cplane_radio_application_header& radio_hdr = cplane_params.radio_hdr;
  ASSERT_EQ(radio_hdr.direction, data_direction::uplink);
  ASSERT_EQ(radio_hdr.slot, context.slot);
  ASSERT_EQ(radio_hdr.filter_index, context.filter_type);
  // Always start in symbol 0.
  ASSERT_EQ(radio_hdr.start_symbol, 0);

  // Check common section 0/1/3/5 fields.
  const cplane_common_section_0_1_3_5_fields& common_fields = cplane_params.section_fields.common_fields;
  // Always use section identifier 0.
  ASSERT_EQ(common_fields.section_id, 0);
  // Always start at PRB 0.
  ASSERT_EQ(common_fields.prb_start, 0);
  ASSERT_EQ(common_fields.nof_prb, context.prach_nof_rb);
  // Use all the RE.
  ASSERT_EQ(common_fields.re_mask, 0xfff);
  ASSERT_EQ(common_fields.nof_symbols, context.nof_repetitions);

  double offset_to_prach_Hz = context.prach_start_re * ra_scs_to_Hz(context.prach_scs);
  double total_bw_Hz        = 1000 * scs_to_khz(context.scs) * ru_nof_prbs * NOF_SUBCARRIERS_PER_RB;
  double freq_offset_Hz     = -(total_bw_Hz / 2 - offset_to_prach_Hz);

  ASSERT_EQ(cplane_params.section_fields.frequency_offset, int(freq_offset_Hz / (ra_scs_to_Hz(context.prach_scs) / 2)));
  ASSERT_EQ(cplane_params.scs, cplane_scs::kHz1_25);
  ASSERT_EQ(cplane_params.cpLength, 0);
  ASSERT_EQ(cplane_params.fft_size, cplane_fft_size::fft_4096);
  ASSERT_EQ(cplane_params.time_offset, context.time_offset);
}

TEST_F(data_flow_cplane_scheduling_commands_impl_fixture,
       calling_enqueue_section_type_3_prach_data_message_adds_context)
{
  data_flow_cplane_scheduling_prach_context context = {{0, 0, 1},
                                                       2,
                                                       filter_index_type::ul_prach_preamble_1p25khz,
                                                       0,
                                                       1,
                                                       srsran::subcarrier_spacing::kHz30,
                                                       srsran::prach_subcarrier_spacing::kHz1_25,
                                                       0,
                                                       72,
                                                       3};

  data_flow.enqueue_section_type_3_prach_message(context);

  // Assert that an entry was created for the uplink Control-Plane message.
  auto repo_context = ul_cplane_context_repo->get(context.slot, 0, context.filter_type, context.eaxc);
  ASSERT_TRUE(repo_context.has_value());

  // Check radio header.
  const cplane_radio_application_header& radio_hdr = repo_context.value().radio_hdr;
  ASSERT_EQ(radio_hdr.direction, data_direction::uplink);
  ASSERT_EQ(radio_hdr.slot, context.slot);
  ASSERT_EQ(radio_hdr.filter_index, context.filter_type);
  // Always start in symbol 0.
  ASSERT_EQ(radio_hdr.start_symbol, context.start_symbol);

  ASSERT_EQ(repo_context.value().nof_symbols, context.nof_repetitions);
  ASSERT_EQ(repo_context.value().nof_prb, context.prach_nof_rb);
  ASSERT_EQ(repo_context.value().prb_start, 0);

  // Check that the context can be found for all the configured symbols.
  for (unsigned i = 0, e = context.nof_repetitions; i != e; ++i) {
    auto ctxt = ul_cplane_context_repo->get(context.slot, i, context.filter_type, context.eaxc);

    ASSERT_TRUE(ctxt.has_value());
  }
}
