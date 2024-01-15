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

#include "../../../../lib/ofh/transmitter/ofh_uplink_request_handler_impl.h"
#include "ofh_data_flow_cplane_scheduling_commands_test_doubles.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;
using namespace srsran::ofh::testing;

static const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> eaxc            = {2};
static const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxc      = {4};
static constexpr unsigned                                    REPOSITORY_SIZE = 20U;

namespace {

/// Spy User-Plane received symbol notifier
class uplane_rx_symbol_notifier_spy : public uplane_rx_symbol_notifier
{
  const resource_grid_reader* rg_reader = nullptr;

public:
  void on_new_uplink_symbol(const uplane_rx_symbol_context& context, const resource_grid_reader& grid) override
  {
    rg_reader = &grid;
  }

  void on_new_prach_window_data(const prach_buffer_context& context, const prach_buffer& buffer) override {}

  const resource_grid_reader* get_reasource_grid_reader() const { return rg_reader; }
};

class prach_buffer_dummy : public prach_buffer
{
  std::array<cf_t, 1> buffer;

public:
  unsigned get_max_nof_ports() const override { return 0; }

  unsigned get_max_nof_td_occasions() const override { return 0; }

  unsigned get_max_nof_fd_occasions() const override { return 0; }

  unsigned get_max_nof_symbols() const override { return 0; }

  unsigned get_sequence_length() const override { return 0; }

  span<cf_t> get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) override
  {
    return buffer;
  }

  span<const cf_t>
  get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) const override
  {
    return buffer;
  }
};

class resource_grid_dummy : public resource_grid
{
  class resource_grid_mapper_dummy : public resource_grid_mapper
  {
  public:
    void
    map(const re_buffer_reader& input, const re_pattern& pattern, const precoding_configuration& precoding) override
    {
    }

    void map(symbol_buffer&                 buffer,
             const re_pattern_list&         pattern,
             const re_pattern_list&         reserved,
             const precoding_configuration& precoding,
             unsigned                       re_skip) override
    {
    }
  };

  class resource_grid_writer_dummy : public resource_grid_writer
  {
  public:
    unsigned get_nof_ports() const override { return 1; }
    unsigned get_nof_subc() const override { return 1; }
    unsigned get_nof_symbols() const override { return 1; }
    void     put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) override {}
    span<const cf_t>
    put(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<const cf_t> symbols) override
    {
      return {};
    }

    span<const cf_t> put(unsigned                            port,
                         unsigned                            l,
                         unsigned                            k_init,
                         const bounded_bitset<NRE * MAX_RB>& mask,
                         span<const cf_t>                    symbols) override
    {
      return {};
    }

    void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override {}
    void put(unsigned port, unsigned l, unsigned k_init, unsigned stride, span<const cf_t> symbols) override {}
  };

  class resource_grid_reader_dummy : public resource_grid_reader
  {
  public:
    unsigned   get_nof_ports() const override { return 1; }
    unsigned   get_nof_subc() const override { return 1; }
    unsigned   get_nof_symbols() const override { return 1; }
    bool       is_empty(unsigned port) const override { return true; }
    bool       is_empty() const override { return true; }
    span<cf_t> get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, span<const bool> mask) const override
    {
      return {};
    }
    span<cf_t> get(span<cf_t>                          symbols,
                   unsigned                            port,
                   unsigned                            l,
                   unsigned                            k_init,
                   const bounded_bitset<MAX_RB * NRE>& mask) const override
    {
      return {};
    }

    void get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init) const override {}

    span<const cf_t> get_view(unsigned port, unsigned l) const override { return {}; }
  };

  resource_grid_reader_dummy reader;
  resource_grid_writer_dummy writer;
  resource_grid_mapper_dummy mapper;

public:
  void set_all_zero() override {}

  resource_grid_writer& get_writer() override { return writer; }

  const resource_grid_reader& get_reader() const override { return reader; }

  resource_grid_mapper& get_mapper() override { return mapper; }
};

class ofh_uplink_request_handler_impl_fixture : public ::testing::Test
{
protected:
  uplink_request_handler_impl_config         cfg;
  std::shared_ptr<uplink_context_repository> ul_slot_repo;
  std::shared_ptr<prach_context_repository>  ul_prach_repo;
  data_flow_cplane_scheduling_commands_spy*  data_flow;
  data_flow_cplane_scheduling_commands_spy*  data_flow_prach;
  uplink_request_handler_impl                handler;
  uplink_request_handler_impl                handler_prach_cp_en;

  explicit ofh_uplink_request_handler_impl_fixture() :
    ul_slot_repo(std::make_shared<uplink_context_repository>(REPOSITORY_SIZE, srslog::fetch_basic_logger("TEST"))),
    ul_prach_repo(std::make_shared<prach_context_repository>(REPOSITORY_SIZE, srslog::fetch_basic_logger("TEST"))),
    handler(get_config_prach_cp_disabled(), get_dependencies_prach_cp_disabled()),
    handler_prach_cp_en(get_config_prach_cp_enabled(), get_dependencies_prach_cp_enabled())
  {
  }

  uplink_request_handler_impl_dependencies get_dependencies_prach_cp_disabled()
  {
    uplink_request_handler_impl_dependencies dependencies;
    dependencies.logger        = &srslog::fetch_basic_logger("TEST");
    dependencies.ul_slot_repo  = ul_slot_repo;
    dependencies.ul_prach_repo = ul_prach_repo;
    auto temp                  = std::make_unique<data_flow_cplane_scheduling_commands_spy>();
    data_flow                  = temp.get();
    dependencies.data_flow     = std::move(temp);

    return dependencies;
  }

  uplink_request_handler_impl_dependencies get_dependencies_prach_cp_enabled()
  {
    uplink_request_handler_impl_dependencies dependencies;
    dependencies.logger        = &srslog::fetch_basic_logger("TEST");
    dependencies.ul_slot_repo  = ul_slot_repo;
    dependencies.ul_prach_repo = ul_prach_repo;
    auto temp                  = std::make_unique<data_flow_cplane_scheduling_commands_spy>();
    data_flow_prach            = temp.get();
    dependencies.data_flow     = std::move(temp);

    return dependencies;
  }

  uplink_request_handler_impl_config get_config_prach_cp_disabled()
  {
    uplink_request_handler_impl_config config;
    config.prach_eaxc          = {};
    config.ul_data_eaxc        = eaxc;
    config.is_prach_cp_enabled = false;
    config.cp                  = cyclic_prefix::NORMAL;

    return config;
  }

  uplink_request_handler_impl_config get_config_prach_cp_enabled()
  {
    uplink_request_handler_impl_config config;
    config.prach_eaxc          = prach_eaxc;
    config.ul_data_eaxc        = {};
    config.is_prach_cp_enabled = true;
    config.cp                  = cyclic_prefix::NORMAL;

    return config;
  }
};

} // namespace

TEST_F(ofh_uplink_request_handler_impl_fixture,
       handle_prach_request_when_cplane_message_is_disable_for_prach_does_not_generate_cplane_message)
{
  prach_buffer_context context;
  context.nof_fd_occasions = 1;
  context.nof_td_occasions = 1;
  context.format           = prach_format_type::B4;
  context.slot             = slot_point(1, 20, 1);
  context.pusch_scs        = subcarrier_spacing::kHz30;
  prach_buffer_dummy buffer_dummy;

  handler.handle_prach_occasion(context, buffer_dummy);

  // Assert data flow.
  ASSERT_FALSE(data_flow->has_enqueue_section_type_1_method_been_called());
  ASSERT_FALSE(data_flow->has_enqueue_section_type_3_method_been_called());
}

TEST_F(ofh_uplink_request_handler_impl_fixture, handle_prach_request_generates_cplane_message)
{
  prach_buffer_context context;
  context.nof_fd_occasions = 1;
  context.nof_td_occasions = 1;
  context.format           = prach_format_type::B4;
  context.slot             = slot_point(1, 20, 1);
  context.pusch_scs        = subcarrier_spacing::kHz30;
  context.start_symbol     = 0;
  prach_buffer_dummy buffer_dummy;

  handler_prach_cp_en.handle_prach_occasion(context, buffer_dummy);

  // Assert data flow.
  ASSERT_FALSE(data_flow_prach->has_enqueue_section_type_1_method_been_called());
  ASSERT_TRUE(data_flow_prach->has_enqueue_section_type_3_method_been_called());

  const data_flow_cplane_scheduling_commands_spy::spy_info& info = data_flow_prach->get_spy_info();
  ASSERT_EQ(context.slot, info.slot);
  ASSERT_EQ(prach_eaxc[0], info.eaxc);
  ASSERT_EQ(data_direction::uplink, info.direction);
  ASSERT_EQ(filter_index_type::ul_prach_preamble_short, info.filter_type);
}

TEST_F(ofh_uplink_request_handler_impl_fixture, handle_uplink_slot_generates_cplane_message)
{
  resource_grid_dummy   rg;
  resource_grid_context rg_context;
  rg_context.slot   = slot_point(1, 1, 1);
  rg_context.sector = 1;

  handler.handle_new_uplink_slot(rg_context, rg);

  // Assert data flow.
  ASSERT_TRUE(data_flow->has_enqueue_section_type_1_method_been_called());
  const data_flow_cplane_scheduling_commands_spy::spy_info& info = data_flow->get_spy_info();
  ASSERT_EQ(rg_context.slot, info.slot);
  ASSERT_EQ(eaxc[0], info.eaxc);
  ASSERT_EQ(data_direction::uplink, info.direction);
}
