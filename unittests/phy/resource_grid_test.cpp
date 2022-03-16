
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/srsvec/zero.h"
#include "srsgnb/support/srsran_assert.h"
#include <map>
#include <random>

using namespace srsgnb;

static std::mt19937 rgen(0);

void test_all_zero(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc)
{
  // Create grid
  std::unique_ptr<resource_grid> grid = create_resource_grid(nof_ports, nof_symbols, nof_subc);

  // Set all grid to zero
  grid->set_all_zero();

  // Verify all RE are zero
  for (unsigned port = 0; port != nof_ports; ++port) {
    for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
      // Get resource grid data for the given symbol
      std::vector<cf_t> rg_data(nof_subc);
      grid->get(rg_data, port, symbol, 0);

      for (unsigned subc = 0; subc != nof_subc; ++subc) {
        cf_t re = rg_data[subc];
        srsran_assert(iszero(re.real()), "Real is not zero");
        srsran_assert(iszero(re.imag()), "Imaginary is not zero");
      }
    }
  }
}

void test_coordinates(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc, unsigned nof_elements)
{
  // Create grid and zero
  std::unique_ptr<resource_grid> grid = create_resource_grid(nof_ports, nof_symbols, nof_subc);
  grid->set_all_zero();

  // Random distributions
  std::uniform_int_distribution<unsigned> port_dist(0, nof_ports - 1);
  std::uniform_int_distribution<uint8_t>  symbol_dist(0, nof_symbols - 1);
  std::uniform_int_distribution<uint16_t> subc_dist(0, nof_subc - 1);
  std::uniform_real_distribution<float>   value_dist(-1.0, +1.0);

  std::vector<resource_grid_coordinate> coordinates(nof_elements);
  std::vector<cf_t>                     symbols_gold(nof_elements);

  // Generate random elements
  unsigned port_gold = port_dist(rgen);
  for (unsigned i = 0; i != nof_elements; ++i) {
    // Generate coordinate, making sure there are no double entries
    bool doubled = false;
    do {
      coordinates[i] = {symbol_dist(rgen), subc_dist(rgen)};

      // Check if the coordinate exists
      for (unsigned j = 0; j != i && !doubled; ++j) {
        doubled = (coordinates[i] == coordinates[j]);
      }
    } while (doubled);

    // Create random value
    symbols_gold[i] = {value_dist(rgen), value_dist(rgen)};
  }

  // Put elements in resource grid
  grid->put(port_gold, coordinates, symbols_gold);

  // Assert grid
  for (unsigned port = 0; port != nof_ports; ++port) {
    for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
      // Get resource grid data for the given symbol
      std::vector<cf_t> rg_data(nof_subc);
      grid->get(rg_data, port, symbol, 0);

      for (unsigned subc = 0; subc != nof_subc; ++subc) {
        cf_t gold  = {0.0, 0.0};
        cf_t value = rg_data[subc];

        // Try to find the RE in coordinates
        resource_grid_coordinate coordinate = {symbol, subc};
        for (unsigned i = 0; i != nof_elements; ++i) {
          if (port == port_gold && coordinates[i] == coordinate) {
            gold = symbols_gold[i];
            break;
          }
        }

        srsran_assert(gold.real() == value.real(), "Unmatched real");
        srsran_assert(gold.imag() == value.imag(), "Unmatched imaginary");
      }
    }
  }

  // Get elements from grid
  std::vector<cf_t> symbols(nof_elements);
  grid->get(symbols, port_gold, coordinates);

  // Assert symbols
  for (unsigned i = 0; i != nof_elements; ++i) {
    cf_t gold  = symbols_gold[i];
    cf_t value = symbols[i];

    srsran_assert(gold.real() == value.real(), "Unmatched real");
    srsran_assert(gold.imag() == value.imag(), "Unmatched imaginary");
  }
}

void test_mask(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc, unsigned nof_elements)
{
  // Create grid and zero
  std::unique_ptr<resource_grid> grid = create_resource_grid(nof_ports, nof_symbols, nof_subc);
  grid->set_all_zero();

  std::uniform_int_distribution<unsigned> port_dist(0, nof_ports - 1);
  std::uniform_int_distribution<unsigned> symbol_dist(0, nof_symbols - 1);
  std::uniform_int_distribution<unsigned> subc_dist(0, nof_subc - 1);
  std::uniform_real_distribution<float>   value_dist(-1.0, +1.0);

  // Put elements in grid
  unsigned                  symbol_idx = symbol_dist(rgen);
  srsvec::aligned_vec<cf_t> symbols_gold(nof_elements);
  srsvec::aligned_vec<bool> mask(nof_subc);

  // Reset mask
  srsvec::zero(mask);

  // Fill mask and generate symbols
  unsigned port_gold = port_dist(rgen);
  for (unsigned i = 0; i != nof_elements; ++i) {
    unsigned subc = 0;

    // Select a subcarrier that has not been set yet
    do {
      subc = subc_dist(rgen);
    } while (mask[subc]);

    // Create random allocation
    mask[subc]      = true;
    symbols_gold[i] = {value_dist(rgen), value_dist(rgen)};
  }

  // Put elements
  span<const cf_t> symbol_buffer_put = grid->put(port_gold, symbol_idx, 0, mask, symbols_gold);

  // Make sure all symbols are used
  srsran_assert(symbol_buffer_put.empty(), "Failed");

  // Assert grid
  unsigned count = 0;
  for (unsigned port = 0; port != nof_ports; ++port) {
    for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
      // Get resource grid data for the given symbol
      std::vector<cf_t> rg_data(nof_subc);
      grid->get(rg_data, port, symbol, 0);

      for (unsigned subc = 0; subc != nof_subc; ++subc) {
        cf_t gold  = {0.0, 0.0};
        cf_t value = rg_data[subc];

        if (port == port_gold && symbol == symbol_idx && mask[subc]) {
          gold = symbols_gold[count];
          count++;
        }

        srsran_assert(gold.real() == value.real(), "Unmatched real");
        srsran_assert(gold.imag() == value.imag(), "Unmatched imaginary");
      }
    }
  }

  // Get elements
  srsvec::aligned_vec<cf_t> symbols(nof_elements);
  span<cf_t>                symbol_buffer_get = grid->get(symbols, port_gold, symbol_idx, 0, mask);

  // Make sure all symbols are used
  srsran_assert(symbol_buffer_get.empty(), "Symbol buffer - not empty");

  // Assert symbols
  for (unsigned i = 0; i != nof_elements; ++i) {
    cf_t gold  = symbols_gold[i];
    cf_t value = symbols[i];

    srsran_assert(gold.real() == value.real(), "Unmatched real");
    srsran_assert(gold.imag() == value.imag(), "Unmatched imaginary");
  }
}

void test_consecutive(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc, unsigned nof_elements)
{
  // Create grid and zero
  std::unique_ptr<resource_grid> grid = create_resource_grid(nof_ports, nof_symbols, nof_subc);
  grid->set_all_zero();

  std::uniform_int_distribution<unsigned> port_dist(0, nof_ports - 1);
  std::uniform_int_distribution<unsigned> symbol_dist(0, nof_symbols - 1);
  std::uniform_int_distribution<unsigned> subc_dist(0, nof_subc - 1 - nof_elements);
  std::uniform_real_distribution<float>   value_dist(-1.0, +1.0);

  // Select port
  unsigned port_gold = port_dist(rgen);

  // Put elements in grid
  unsigned                  symbol_idx = symbol_dist(rgen);
  srsvec::aligned_vec<cf_t> symbols_gold(nof_elements);

  // Select initial subcarrier
  unsigned k_init = subc_dist(rgen);

  // Create random data
  for (unsigned i = 0; i != nof_elements; ++i) {
    symbols_gold[i] = {value_dist(rgen), value_dist(rgen)};
  }

  // Put element
  grid->put(port_gold, symbol_idx, k_init, symbols_gold);

  // Assert grid
  unsigned count = 0;
  for (unsigned port = 0; port != nof_ports; ++port) {
    for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
      // Get resource grid data for the given symbol
      std::vector<cf_t> rg_data(nof_subc);
      grid->get(rg_data, port, symbol, 0);

      for (unsigned subc = 0; subc != nof_subc; ++subc) {
        cf_t gold  = {0.0, 0.0};
        cf_t value = rg_data[subc];

        if (port == port_gold && symbol == symbol_idx && (subc >= k_init && subc < k_init + nof_elements)) {
          gold = symbols_gold[count];
          count++;
        }

        srsran_assert(gold.real() == value.real(), "Unmatched real");
        srsran_assert(gold.imag() == value.imag(), "Unmatched imaginary");
      }
    }
  }

  // Get elements
  srsvec::aligned_vec<cf_t> symbols(nof_elements);
  grid->get(symbols, port_gold, symbol_idx, k_init);

  // Assert symbols
  for (unsigned i = 0; i != nof_elements; ++i) {
    cf_t gold  = symbols_gold[i];
    cf_t value = symbols[i];

    srsran_assert(gold.real() == value.real(), "Unmatched real");
    srsran_assert(gold.imag() == value.imag(), "Unmatched imaginary");
  }
}

int main()
{
  // Iterates over the possible number of ports
  for (unsigned nof_ports : {1, 2, 4, 8, 16}) {
    // Iterate over the posisble number of symbols per slot
    for (unsigned nof_symbols : {14}) {
      // Iterate over a symbolic number of subcarriers
      for (unsigned nof_subc : {6 * 12, 15 * 12}) {
        test_all_zero(nof_ports, nof_symbols, nof_subc);
        // Test symbolic number of elements
        for (unsigned nof_elements : {1, 2, 4, 8, 16}) {
          test_coordinates(nof_ports, nof_symbols, nof_subc, nof_elements);
          test_mask(nof_ports, nof_symbols, nof_subc, nof_elements);
          test_consecutive(nof_ports, nof_symbols, nof_subc, nof_elements);
        }
      }
    }
  }

  return 0;
}
