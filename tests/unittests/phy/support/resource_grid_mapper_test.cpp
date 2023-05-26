/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/constants.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/precoding/precoding_factories.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/srsvec/zero.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

// Pseudo-random number generator.
static std::mt19937 rgen;

using MultiplePRGParams = std::tuple<
    // Number of RB.
    unsigned,
    // Number of antenna ports.
    unsigned,
    // PRG size in number of RB.
    unsigned>;

// Asserts that the contents of the resource grid match the golden symbols for the give allocation pattern.
static void assert_grid(const re_buffer_reader& golden, re_pattern_list& allocation, const resource_grid_reader& grid)
{
  unsigned nof_ports = golden.get_nof_slices();
  for (unsigned i_symbol = 0, re_offset = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
    // RE mask for an OFDM symbol.
    bounded_bitset<MAX_RB * NRE> re_mask(MAX_RB * NRE);
    allocation.get_inclusion_mask(re_mask, i_symbol);

    // View of the golden data for an OFDM symbol.
    unsigned              re_count = re_mask.count();
    re_buffer_reader_view golden_symbol(golden, re_offset, re_count);

    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      // Get the precoded and mapped symbols from the grid.
      std::vector<cf_t> grid_symbols(re_count);
      grid.get(grid_symbols, i_port, i_symbol, 0, re_mask);

      // Assert symbols.
      ASSERT_EQ(span<const cf_t>(golden_symbol.get_slice(i_port)), span<const cf_t>(grid_symbols));

      // Get the rest of the symbols from the grid.
      std::vector<cf_t> grid_zeros(re_mask.size() - re_count);

      // Golden sequence with all its elements set to zero.
      std::vector<cf_t> golden_zeros(re_mask.size() - re_count);
      srsvec::zero(golden_zeros);

      grid.get(grid_zeros, i_port, i_symbol, 0, ~re_mask);

      // Assert that all the grid locations not included in the grid allocation are set to zero.
      ASSERT_EQ(span<const cf_t>(golden_zeros), span<const cf_t>(grid_zeros));
    }

    // Advance views.
    re_offset += re_count;
  }
}

namespace srsran {

static float ASSERT_MAX_ERROR = 1e-6;

static std::ostream& operator<<(std::ostream& os, span<const cf_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

static bool operator==(span<const cf_t> lhs, span<const cf_t> rhs)
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](cf_t lhs_val, cf_t rhs_val) {
    return (std::abs(lhs_val - rhs_val) < ASSERT_MAX_ERROR);
  });
}

} // namespace srsran

namespace {
class ResourceGridMapperFixture : public ::testing::TestWithParam<MultiplePRGParams>
{
protected:
  static std::shared_ptr<channel_precoder_factory> precoder_factory;
  static std::unique_ptr<channel_precoder>         precoder;
  static std::unique_ptr<resource_grid>            grid;

  static void SetUpTestSuite()
  {
    // Create channel precoder factory.
    if (!precoder_factory) {
      precoder_factory = create_channel_precoder_factory();
      ASSERT_NE(precoder_factory, nullptr) << "Cannot create channel precoder factory";
    }
  }

  void SetUp() override
  {
    ASSERT_NE(precoder_factory, nullptr) << "Cannot create channel precoder factory";

    // Create channel precoder.
    precoder = precoder_factory->create();
    ASSERT_NE(precoder, nullptr) << "Cannot create channel precoder";

    // Create resource grid.
    grid = create_resource_grid(precoding_constants::MAX_NOF_PORTS, MAX_NSYMB_PER_SLOT, MAX_RB * NRE);
    grid->set_all_zero();
  }

  // Generates and returns random RE values, as many as the specified number of layers and RE.
  const re_buffer_reader& generate_random_data(unsigned nof_layers, unsigned nof_re)
  {
    // Resize buffer.
    random_data.resize(nof_layers, nof_re);

    // Fill buffer with random values.
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      for (cf_t& val : random_data.get_slice(i_layer)) {
        val = {data_dist(rgen), data_dist(rgen)};
      }
    }

    return random_data;
  }

  // Generates and returns random RE values, as many as the specified number of layers and RE.
  static re_pattern_list generate_random_allocation(unsigned nof_layers, unsigned nof_re_rb, unsigned nof_rb_grid)
  {
    interval<unsigned, true> rb_range(1, MAX_RB);
    interval<unsigned, true> re_rb_range(1, NRE);

    srsran_assert(
        rb_range.contains(nof_rb_grid), "The number of RBs (i.e., {}) is out of range {}.", nof_rb_grid, rb_range);

    srsran_assert(re_rb_range.contains(nof_re_rb),
                  "The number of RE per RB (i.e., {}) is out of range {}.",
                  nof_re_rb,
                  re_rb_range);

    // Some RE.
    re_prb_mask re_mask;
    unsigned    stride = NRE / nof_re_rb;
    for (unsigned i_re = 0, inserted_re = 0; inserted_re != nof_re_rb; ++inserted_re, i_re += stride) {
      re_mask.set(i_re);
    }

    // All symbols.
    symbol_slot_mask symbols = ~symbol_slot_mask(MAX_NSYMB_PER_SLOT);

    // All RBs.
    re_pattern allocation(0, nof_rb_grid, 1, re_mask, symbols);

    re_pattern_list allocation_list(allocation);
    return allocation_list;
  }

  // Generates the golden RE sequence, with the dimensions specified by the input buffer and precoding configuration.
  const re_buffer_reader& generate_golden(const re_buffer_reader& input, const precoding_configuration& configuration)
  {
    // Get dimensions.
    unsigned nof_re      = input.get_nof_re();
    unsigned nof_layers  = configuration.get_nof_layers();
    unsigned nof_ports   = configuration.get_nof_ports();
    unsigned nof_prg     = configuration.get_nof_prg();
    unsigned nof_re_prg  = configuration.get_prg_size() * NRE;
    unsigned nof_symbols = nof_re / (nof_re_prg * nof_prg);

    EXPECT_EQ(nof_layers, input.get_nof_slices()) << "Input and precoding matrix number of layers don't match.";

    // Resize buffer.
    golden_data.resize(nof_ports, nof_re);

    unsigned prg_re_offset = 0;
    for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
      for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
        // Number of data RE that belong to the current PRG.
        unsigned re_count = std::min(nof_re_prg, nof_re - prg_re_offset);

        // View over the input data for a single PRG.
        re_buffer_reader_view input_prg(input, prg_re_offset, re_count);

        // View over the golden sequence for a single PRG.
        re_buffer_writer_view golden_prg(golden_data, prg_re_offset, re_count);

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          // View over the golden sequence for a single PRG and antenna port.
          span<cf_t> golden_re_port = golden_prg.get_slice(i_port);

          // Set the output REs to zero.
          srsvec::zero(golden_re_port);

          for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
            // View over the input data for a single PRG and layer.
            span<const cf_t> input_re_layer = input_prg.get_slice(i_layer);

            for (unsigned i_re = 0; i_re != re_count; ++i_re) {
              // Accumulate the contributions of all layers, scaled by each respective precoding weight.
              golden_re_port[i_re] += configuration.get_coefficient(i_layer, i_port, i_prg) * input_re_layer[i_re];
            }
          }
        }

        // Advance buffers.
        prg_re_offset += re_count;
      }
    }

    EXPECT_EQ(nof_re, prg_re_offset)
        << "The number of RE of the golden sequence does not match the number of input RE.";

    return golden_data;
  }

  // Generates a precoding configuration with the specified dimensions and random precoding coefficients.
  precoding_configuration
  generate_random_precoding(unsigned nof_layers, unsigned nof_ports, unsigned nof_prg, unsigned prg_size)
  {
    // Create configuration.
    precoding_configuration config(nof_layers, nof_ports, nof_prg, prg_size);

    // Fill the precoding matrices with random values.
    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          cf_t weight = {weight_dist(rgen), weight_dist(rgen)};
          config.set_coefficient(weight, i_layer, i_port, i_prg);
        }
      }
    }

    return config;
  }

private:
  // Buffer for holding randomly generated data.
  static_re_buffer<precoding_constants::MAX_NOF_LAYERS, NRE * MAX_RB * MAX_NSYMB_PER_SLOT> random_data;

  // Buffer for holding the golden sequence.
  static_re_buffer<precoding_constants::MAX_NOF_PORTS, NRE * MAX_RB * MAX_NSYMB_PER_SLOT> golden_data;

  // Uniform distribution to generate data samples.
  std::uniform_real_distribution<float> data_dist{-10.0, +10.0};

  // Uniform distribution to generate precoding coefficients.
  std::uniform_real_distribution<float> weight_dist{-1, +1};
};

std::shared_ptr<channel_precoder_factory> ResourceGridMapperFixture::precoder_factory = nullptr;
std::unique_ptr<channel_precoder>         ResourceGridMapperFixture::precoder         = nullptr;
std::unique_ptr<resource_grid>            ResourceGridMapperFixture::grid             = nullptr;

// Tests a wideband single port precoding configuration, generated by the function make_single_port.
TEST_F(ResourceGridMapperFixture, SinglePort)
{
  constexpr unsigned nof_layers = 1;

  // Create a single port precoding configuration.
  precoding_configuration precoding_config = make_single_port();

  // Number of RB from the precoding configuration.
  unsigned nof_rb = precoding_config.get_nof_prg() * precoding_config.get_prg_size();

  // Number of resource elements per layer.
  unsigned nof_re = nof_rb * NRE * MAX_NSYMB_PER_SLOT;

  // Generate resource grid allocation.
  re_pattern_list allocation = generate_random_allocation(nof_layers, NRE, nof_rb);

  // Generate random RE arranged by layers.
  const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_re);

  // Get the resource grid mapper.
  resource_grid_mapper& mapper = grid->get_mapper();

  // Map into the resource grid.
  mapper.map(input_data, allocation, precoding_config);

  // Generate the golden precoded data.
  const re_buffer_reader& golden = generate_golden(input_data, precoding_config);

  // Assert resource grid contents.
  assert_grid(golden, allocation, *grid);
}

// Tests a wideband, one layer to one antenna port configuration, generated by the function
// make_wideband_one_layer_one_port.
TEST_F(ResourceGridMapperFixture, OneLayerToOnePort)
{
  constexpr unsigned nof_layers = 1;

  for (unsigned nof_ports = 1; nof_ports <= precoding_constants::MAX_NOF_PORTS; ++nof_ports) {
    // The precoding configuration maps a single layer to one of the antenna ports, selected randomly.
    std::uniform_int_distribution<unsigned> selected_port_dist{0, nof_ports - 1};

    // Create a single port precoding configuration.
    precoding_configuration precoding_config = make_wideband_one_layer_one_port(nof_ports, selected_port_dist(rgen));

    // Number of RB from the precoding configuration.
    unsigned nof_rb = precoding_config.get_nof_prg() * precoding_config.get_prg_size();

    // Number of resource elements per layer.
    unsigned nof_re = nof_rb * NRE * MAX_NSYMB_PER_SLOT;

    // Generate resource grid allocation.
    re_pattern_list allocation = generate_random_allocation(nof_layers, NRE, nof_rb);

    // Generate random RE arranged by layers.
    const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_re);

    // Get the resource grid mapper.
    resource_grid_mapper& mapper = grid->get_mapper();

    // Map into the resource grid.
    mapper.map(input_data, allocation, precoding_config);

    // Generate the golden precoded data.
    const re_buffer_reader& golden = generate_golden(input_data, precoding_config);

    // Assert resource grid contents.
    assert_grid(golden, allocation, *grid);
  }
}
// Tests a wideband, identity matrix precoding configuration, generated by the function make_wideband_identity.
TEST_F(ResourceGridMapperFixture, Identity)
{
  for (unsigned nof_streams = 1; nof_streams <= precoding_constants::MAX_NOF_LAYERS; ++nof_streams) {
    // Create a precoding configuration with precoding coefficients forming an identity matrix.
    precoding_configuration precoding_config = make_wideband_identity(nof_streams);

    // Number of RB from the precoding configuration.
    unsigned nof_rb = precoding_config.get_nof_prg() * precoding_config.get_prg_size();

    // Number of resource elements per layer.
    unsigned nof_re = nof_rb * NRE * MAX_NSYMB_PER_SLOT;

    // Generate resource grid allocation.
    re_pattern_list allocation = generate_random_allocation(nof_streams, NRE, nof_rb);

    // Generate random RE arranged by layers.
    const re_buffer_reader& input_data = generate_random_data(nof_streams, nof_re);

    // Get the resource grid mapper.
    resource_grid_mapper& mapper = grid->get_mapper();

    // Map into the resource grid.
    mapper.map(input_data, allocation, precoding_config);

    // Generate the golden precoded data.
    const re_buffer_reader& golden = generate_golden(input_data, precoding_config);

    // Assert resource grid contents.
    assert_grid(golden, allocation, *grid);
  }
}

// Tests a wideband, one layer to two antenna ports configuration, generated by the function
// make_wideband_one_layer_two_ports.
TEST_F(ResourceGridMapperFixture, OneLayerTwoPorts)
{
  constexpr unsigned nof_layers = 1;

  for (unsigned codebook_index = 0, nof_cb_index = 4; codebook_index != nof_cb_index; ++codebook_index) {
    // Create a single layer to two ports precoding configuration.
    precoding_configuration precoding_config = make_wideband_one_layer_two_ports(codebook_index);

    // Number of RB from the precoding configuration.
    unsigned nof_rb = precoding_config.get_nof_prg() * precoding_config.get_prg_size();

    // Number of resource elements per layer.
    unsigned nof_re = nof_rb * NRE * MAX_NSYMB_PER_SLOT;

    // Generate resource grid allocation.
    re_pattern_list allocation = generate_random_allocation(nof_layers, NRE, nof_rb);

    // Generate random RE arranged by layers.
    const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_re);

    // Get the resource grid mapper.
    resource_grid_mapper& mapper = grid->get_mapper();

    // Map into the resource grid.
    mapper.map(input_data, allocation, precoding_config);

    // Generate the golden precoded data.
    const re_buffer_reader& golden = generate_golden(input_data, precoding_config);

    // Assert resource grid contents.
    assert_grid(golden, allocation, *grid);
  }
}

// Tests a wideband, two layers to two antenna ports configuration, generated by the function
// make_wideband_two_layer_two_ports.
TEST_F(ResourceGridMapperFixture, TwoLayerTwoPorts)
{
  constexpr unsigned nof_layers = 2;

  for (unsigned codebook_index = 0, nof_cb_index = 2; codebook_index != nof_cb_index; ++codebook_index) {
    // Create a single layer to two ports precoding configuration.
    precoding_configuration precoding_config = make_wideband_two_layer_two_ports(codebook_index);

    // Number of RB from the precoding configuration.
    unsigned nof_rb = precoding_config.get_nof_prg() * precoding_config.get_prg_size();

    // Number of resource elements per layer.
    unsigned nof_re = nof_rb * NRE * MAX_NSYMB_PER_SLOT;

    // Generate resource grid allocation.
    re_pattern_list allocation = generate_random_allocation(nof_layers, NRE, nof_rb);

    // Generate random RE arranged by layers.
    const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_re);

    // Get the resource grid mapper.
    resource_grid_mapper& mapper = grid->get_mapper();

    // Map into the resource grid.
    mapper.map(input_data, allocation, precoding_config);

    // Generate the golden precoded data.
    const re_buffer_reader& golden = generate_golden(input_data, precoding_config);

    // Assert resource grid contents.
    assert_grid(golden, allocation, *grid);
  }
}

// Tests a precoding configuration with parametrized dimensions and random coefficients.
TEST_P(ResourceGridMapperFixture, MultiplePrg)
{
  const MultiplePRGParams& test_case = GetParam();

  unsigned nof_rb    = std::get<0>(test_case);
  unsigned nof_ports = std::get<1>(test_case);
  unsigned prg_size  = std::get<2>(test_case);

  // Number of subcarriers in an OFDM symbol.
  unsigned nof_subc = nof_rb * NRE;

  // Number of resource elements per layer in a slot.
  unsigned nof_re_slot = nof_subc * MAX_NSYMB_PER_SLOT;

  // Number of RE that fit in a PRG for an OFDM symbol.
  unsigned nof_re_prg = prg_size * NRE;

  for (unsigned nof_layers = 1; nof_layers <= nof_ports; ++nof_layers) {
    // Number of PRG that the data spans.
    unsigned nof_prg = divide_ceil(nof_subc, nof_re_prg);

    // Create a random precoding configuration
    precoding_configuration precoding_config = generate_random_precoding(nof_layers, nof_ports, nof_prg, prg_size);

    // Generate resource grid allocation.
    re_pattern_list allocation = generate_random_allocation(nof_layers, NRE, nof_rb);

    // Generate random RE arranged by layers.
    const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_re_slot);

    // Get the resource grid mapper.
    resource_grid_mapper& mapper = grid->get_mapper();

    // Map into the resource grid.
    mapper.map(input_data, allocation, precoding_config);

    // Generate the golden precoded data.
    const re_buffer_reader& golden = generate_golden(input_data, precoding_config);

    // Assert resource grid contents.
    assert_grid(golden, allocation, *grid);
  }
}

INSTANTIATE_TEST_SUITE_P(MultiplePrg,
                         ResourceGridMapperFixture,
                         ::testing::Combine(
                             // Number of RB.
                             ::testing::Values(13, 51, MAX_RB),
                             // Number of antenna ports.
                             ::testing::Values(1, 2, 4),
                             // PRG size in number of RB.
                             ::testing::Values(4, 13, 52, MAX_RB)));
} // namespace