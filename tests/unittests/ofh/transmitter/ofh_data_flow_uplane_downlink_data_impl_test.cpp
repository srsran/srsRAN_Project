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

#include "../../../../lib/ofh/transmitter/ofh_data_flow_uplane_downlink_data_impl.h"
#include "../../phy/support/resource_grid_test_doubles.h"
#include "../compression/ofh_iq_compressor_test_doubles.h"
#include "../ecpri/ecpri_packet_builder_test_doubles.h"
#include "../ethernet/vlan_ethernet_frame_builder_test_doubles.h"
#include "srsran/adt/interval.h"
#include "srsran/ofh/ethernet/ethernet_frame_pool.h"
#include "srsran/phy/support/resource_grid_context.h"
#include <gtest/gtest.h>
#include <vector>

using namespace srsran;
using namespace ofh;

namespace {

/// Spy OFH User-Plane packet builder.
class ofh_uplane_packet_builder_spy : public uplane_message_builder
{
  static_vector<uplane_message_params, MAX_NSYMB_PER_SLOT> uplane_msg_params;
  std::vector<std::vector<cf_t>>                           iq_data;

public:
  ofh_uplane_packet_builder_spy()
  {
    iq_data.resize(MAX_NSYMB_PER_SLOT);
    for (auto& iq_symbol : iq_data) {
      iq_symbol.resize(MAX_NOF_PRBS * NOF_SUBCARRIERS_PER_RB);
    }
  }

  units::bytes get_header_size(const ru_compression_params& params) const override { return units::bytes(8); }

  unsigned build_message(span<uint8_t> buffer, span<const cf_t> grid, const uplane_message_params& params) override
  {
    std::copy(grid.begin(), grid.end(), iq_data[params.symbol_id].begin() + params.start_prb * NOF_SUBCARRIERS_PER_RB);
    uplane_msg_params.push_back(params);

    return 0;
  }

  /// Returns the number of built packets.
  unsigned nof_built_packets() const { return uplane_msg_params.size(); }

  /// Retuns a span of the User-Plane message parameters processed by this builder.
  span<const uplane_message_params> get_uplane_params() const { return uplane_msg_params; }

  /// Returns a pointer to the resource grid reader processed by this builder.
  span<const cf_t> get_iq_data(unsigned symbol) const { return iq_data[symbol]; }
};

} // namespace

class ofh_data_flow_uplane_downlink_data_impl_fixture : public ::testing::TestWithParam<ru_compression_params>
{
protected:
  const unsigned                          nof_symbols;
  const unsigned                          ru_nof_prbs;
  const unsigned                          du_nof_prbs;
  const ether::vlan_frame_params          vlan_params  = {{0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x11},
                                                          {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x22},
                                                          1,
                                                          0xaabb};
  const ru_compression_params             compr_params = GetParam();
  data_flow_uplane_downlink_data_impl     data_flow;
  ether::testing::vlan_frame_builder_spy* vlan_builder;
  ecpri::testing::packet_builder_spy*     ecpri_builder;
  ofh_uplane_packet_builder_spy*          uplane_builder;
  resource_grid_reader_spy                rg_reader_spy;

  ofh_data_flow_uplane_downlink_data_impl_fixture() :
    nof_symbols(3),
    ru_nof_prbs(273),
    du_nof_prbs(273),
    data_flow(get_config(), generate_data_flow_dependencies()),
    rg_reader_spy(1, nof_symbols, du_nof_prbs)
  {
    initialize_grid_reader();
  }

  data_flow_uplane_downlink_data_impl_config get_config()
  {
    data_flow_uplane_downlink_data_impl_config config;
    config.ru_nof_prbs  = ru_nof_prbs;
    config.vlan_params  = vlan_params;
    config.compr_params = compr_params;

    return config;
  };

  data_flow_uplane_downlink_data_impl_dependencies generate_data_flow_dependencies()
  {
    data_flow_uplane_downlink_data_impl_dependencies dependencies;
    dependencies.logger         = &srslog::fetch_basic_logger("TEST");
    dependencies.compressor_sel = std::make_unique<ofh::testing::iq_compressor_dummy>();
    dependencies.frame_pool     = std::make_shared<ether::eth_frame_pool>(units::bytes(9000), 2);

    {
      auto temp               = std::make_unique<ofh_uplane_packet_builder_spy>();
      uplane_builder          = temp.get();
      dependencies.up_builder = std::move(temp);
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

  void initialize_grid_reader()
  {
    for (uint8_t symbol = 0; symbol != MAX_NSYMB_PER_SLOT; ++symbol) {
      for (uint16_t k = 0, e = MAX_NOF_PRBS * NOF_SUBCARRIERS_PER_RB; k != e; ++k) {
        rg_reader_spy.write(
            resource_grid_reader_spy::expected_entry_t{0, symbol, k, (k > 200) ? cf_t{1, 1} : cf_t{1, 0}});
      }
    }
  }
};

static const std::array<ru_compression_params, 2> compr_params = {
    {{compression_type::none, 16}, {compression_type::BFP, 9}}};
static const std::array<std::vector<interval<unsigned>>, 2> segmented_prbs = {{{{0, 186}, {186, 273}}, {{0, 273}}}};

INSTANTIATE_TEST_SUITE_P(compression_params,
                         ofh_data_flow_uplane_downlink_data_impl_fixture,
                         ::testing::ValuesIn(compr_params));

TEST_P(ofh_data_flow_uplane_downlink_data_impl_fixture, calling_enqueue_section_type_1_message_success)
{
  data_flow_uplane_resource_grid_context context;
  context.port         = 0;
  context.sector       = 0;
  context.slot         = slot_point(0, 0, 0);
  context.eaxc         = 2;
  context.symbol_range = {0, 3};

  data_flow.enqueue_section_type_1_message(context, rg_reader_spy);

  // Assert VLAN parameters.
  const ether::vlan_frame_params& vlan = vlan_builder->get_vlan_frame_params();
  ASSERT_TRUE(vlan_builder->has_build_vlan_frame_method_been_called());
  ASSERT_EQ(vlan_params.eth_type, vlan.eth_type);
  ASSERT_EQ(vlan_params.mac_dst_address, vlan.mac_dst_address);
  ASSERT_EQ(vlan_params.mac_src_address, vlan.mac_src_address);
  ASSERT_EQ(vlan_params.tci, vlan.tci);

  // Assert eCPRI parameters.
  ASSERT_TRUE(ecpri_builder->has_build_data_packet_method_been_called());
  ASSERT_FALSE(ecpri_builder->has_build_control_packet_method_been_called());
  // Assert there is only one packet per symbol.
  span<const ecpri::iq_data_parameters> data_params = ecpri_builder->get_data_parameters();
  ASSERT_EQ(data_params.size(), nof_symbols * segmented_prbs[static_cast<unsigned>(compr_params.type)].size());
  sequence_identifier_generator generator;
  for (const auto& param : data_params) {
    ASSERT_EQ(param.seq_id >> 8U, generator.generate(context.eaxc));
    ASSERT_EQ(param.pc_id, context.eaxc);
  }

  // Assert Open Fronthaul parameters.
  span<const uplane_message_params>      uplane_params = uplane_builder->get_uplane_params();
  const std::vector<interval<unsigned>>& seg_prbs      = segmented_prbs[static_cast<unsigned>(compr_params.type)];
  ASSERT_EQ(uplane_builder->nof_built_packets(), nof_symbols * seg_prbs.size());

  unsigned symbol_id = 0;
  unsigned prb_index = 0;
  for (const auto& param : uplane_params) {
    ASSERT_EQ(param.direction, data_direction::downlink);
    ASSERT_EQ(param.slot, context.slot);
    ASSERT_EQ(param.filter_index, filter_index_type::standard_channel_filter);
    ASSERT_EQ(param.start_prb, seg_prbs[prb_index].start());
    ASSERT_EQ(param.nof_prb, seg_prbs[prb_index].length());
    ASSERT_EQ(param.symbol_id, symbol_id);
    ASSERT_EQ(param.sect_type, section_type::type_1);
    ASSERT_EQ(param.compression_params.data_width, compr_params.data_width);
    ASSERT_EQ(param.compression_params.type, compr_params.type);

    // Check the symbols.
    std::vector<cf_t> iq_symbols(param.nof_prb * NOF_SUBCARRIERS_PER_RB, 0);
    rg_reader_spy.get(iq_symbols, 0, symbol_id, param.start_prb * NOF_SUBCARRIERS_PER_RB);

    ASSERT_EQ(span<const cf_t>(iq_symbols),
              uplane_builder->get_iq_data(symbol_id).subspan(param.start_prb * NOF_SUBCARRIERS_PER_RB,
                                                             param.nof_prb * NOF_SUBCARRIERS_PER_RB));

    // Increase the segmented PRB index.
    prb_index = (prb_index + 1) % seg_prbs.size();
    // Increase symbol index when all the PRBs for a symbol have been checked.
    if (prb_index == 0) {
      ++symbol_id;
    }
  }
}

TEST(ofh_data_flow_uplane_downlink_data_impl,
     frame_buffer_size_of_nof_prbs_plus_headers_size_generates_one_packet_per_symbol)
{
  data_flow_uplane_resource_grid_context context;
  context.port         = 0;
  context.sector       = 0;
  context.slot         = slot_point(0, 0, 0);
  context.eaxc         = 2;
  context.symbol_range = {0, 3};

  data_flow_uplane_downlink_data_impl_config config;

  config.ru_nof_prbs  = 273;
  config.vlan_params  = {{0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x11}, {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x22}, 1, 0xaabb};
  config.compr_params = {compression_type::BFP, 9};

  data_flow_uplane_downlink_data_impl_dependencies dependencies;
  dependencies.logger         = &srslog::fetch_basic_logger("TEST");
  dependencies.compressor_sel = std::make_unique<ofh::testing::iq_compressor_dummy>();

  unsigned prb_size   = 28;
  unsigned frame_size = prb_size * config.ru_nof_prbs;

  ofh_uplane_packet_builder_spy* uplane_builder;

  {
    auto temp = std::make_unique<ofh_uplane_packet_builder_spy>();
    frame_size += temp->get_header_size(config.compr_params).value();
    uplane_builder          = temp.get();
    dependencies.up_builder = std::move(temp);
  }
  {
    auto temp = std::make_unique<ether::testing::vlan_frame_builder_spy>();
    frame_size += temp->get_header_size().value();
    dependencies.eth_builder = std::move(temp);
  }
  {
    auto temp = std::make_unique<ecpri::testing::packet_builder_spy>();
    frame_size += temp->get_header_size(ecpri::message_type::iq_data).value();
    dependencies.ecpri_builder = std::move(temp);
  }

  dependencies.frame_pool = std::make_shared<ether::eth_frame_pool>(units::bytes(frame_size), 2);

  resource_grid_reader_spy rg_reader_spy(1, context.symbol_range.length(), config.ru_nof_prbs);
  for (uint8_t symbol = 0; symbol != MAX_NSYMB_PER_SLOT; ++symbol) {
    for (uint16_t k = 0, e = MAX_NOF_PRBS * NOF_SUBCARRIERS_PER_RB; k != e; ++k) {
      rg_reader_spy.write(
          resource_grid_reader_spy::expected_entry_t{0, symbol, k, (k > 200) ? cf_t{1, 1} : cf_t{1, 0}});
    }
  }

  data_flow_uplane_downlink_data_impl data_flow(config, std::move(dependencies));
  data_flow.enqueue_section_type_1_message(context, rg_reader_spy);

  // Assert number of packets.
  ASSERT_EQ(uplane_builder->nof_built_packets(), context.symbol_range.length());
}

TEST(ofh_data_flow_uplane_downlink_data_impl, frame_buffer_size_of_nof_prbs_generates_two_packets_per_symbol)
{
  data_flow_uplane_resource_grid_context context;
  context.port         = 0;
  context.sector       = 0;
  context.slot         = slot_point(0, 0, 0);
  context.eaxc         = 2;
  context.symbol_range = {0, 3};

  data_flow_uplane_downlink_data_impl_config config;

  config.ru_nof_prbs  = 273;
  config.vlan_params  = {{0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x11}, {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x22}, 1, 0xaabb};
  config.compr_params = {compression_type::BFP, 9};

  data_flow_uplane_downlink_data_impl_dependencies dependencies;
  dependencies.logger         = &srslog::fetch_basic_logger("TEST");
  dependencies.compressor_sel = std::make_unique<ofh::testing::iq_compressor_dummy>();

  unsigned prb_size  = 28;
  unsigned prbs_size = prb_size * config.ru_nof_prbs;

  dependencies.frame_pool = std::make_shared<ether::eth_frame_pool>(units::bytes(prbs_size), 2);
  ofh_uplane_packet_builder_spy* uplane_builder;

  {
    auto temp               = std::make_unique<ofh_uplane_packet_builder_spy>();
    uplane_builder          = temp.get();
    dependencies.up_builder = std::move(temp);
  }
  {
    auto temp                = std::make_unique<ether::testing::vlan_frame_builder_spy>();
    dependencies.eth_builder = std::move(temp);
  }
  {
    auto temp                  = std::make_unique<ecpri::testing::packet_builder_spy>();
    dependencies.ecpri_builder = std::move(temp);
  }
  resource_grid_reader_spy rg_reader_spy(1, context.symbol_range.length(), config.ru_nof_prbs);
  for (uint8_t symbol = 0; symbol != MAX_NSYMB_PER_SLOT; ++symbol) {
    for (uint16_t k = 0, e = MAX_NOF_PRBS * NOF_SUBCARRIERS_PER_RB; k != e; ++k) {
      rg_reader_spy.write(
          resource_grid_reader_spy::expected_entry_t{0, symbol, k, (k > 200) ? cf_t{1, 1} : cf_t{1, 0}});
    }
  }

  data_flow_uplane_downlink_data_impl data_flow(config, std::move(dependencies));
  data_flow.enqueue_section_type_1_message(context, rg_reader_spy);

  // Assert number of packets. As the packet should not fit in the frame, check that it generated 2 packets per symbol.
  ASSERT_EQ(uplane_builder->nof_built_packets(), context.symbol_range.length() * 2);
}
