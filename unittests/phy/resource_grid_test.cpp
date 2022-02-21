
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/srsvec/zero.h"
#include <cassert>
#include <map>
#include <random>

using namespace srsgnb;

static std::mt19937 rgen(0);

void test_all_zero(unsigned nof_symbols, unsigned nof_subc)
{
  // Create grid
  std::unique_ptr<resource_grid> grid = create_resource_grid(nof_symbols, nof_subc);

  // Set all grid to zero
  grid->all_zero();

  // Verify all RE are zero
  for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
    // Get resource grid data for the given symbol
    std::vector<cf_t> rg_data(nof_subc);
    grid->get(symbol, 0, rg_data);

    for (unsigned subc = 0; subc != nof_subc; ++subc) {
      cf_t re = rg_data[subc];
      assert(iszero(re.real()));
      assert(iszero(re.imag()));
    }
  }
}

void test_coordinates(unsigned nof_symbols, unsigned nof_subc, unsigned nof_elements)
{
  // Create grid and zero
  std::unique_ptr<resource_grid> grid = create_resource_grid(nof_symbols, nof_subc);
  grid->all_zero();

  // Random distributions
  std::uniform_int_distribution<uint8_t>  symbol_dist(0, nof_symbols - 1);
  std::uniform_int_distribution<uint16_t> subc_dist(0, nof_subc - 1);
  std::uniform_real_distribution<float>   value_dist(-1.0, +1.0);

  std::vector<resource_grid_coordinate> coordinates(nof_elements);
  std::vector<cf_t>                     symbols_gold(nof_elements);

  // Generate random elements
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
  grid->put(coordinates, symbols_gold);

  // Assert grid
  for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
    // Get resource grid data for the given symbol
    std::vector<cf_t> rg_data(nof_subc);
    grid->get(symbol, 0, rg_data);

    for (unsigned subc = 0; subc != nof_subc; ++subc) {
      cf_t gold  = {0.0, 0.0};
      cf_t value = rg_data[subc];

      // Try to find the RE in coordinates
      resource_grid_coordinate coordinate = {symbol, subc};
      for (unsigned i = 0; i != nof_elements; ++i) {
        if (coordinates[i] == coordinate) {
          gold = symbols_gold[i];
          break;
        }
      }

      assert(gold.real() == value.real());
      assert(gold.imag() == value.imag());
    }
  }

  // Get elements from grid
  std::vector<cf_t> symbols(nof_elements);
  grid->get(coordinates, symbols);

  // Assert symbols
  for (unsigned i = 0; i != nof_elements; ++i) {
    cf_t gold  = symbols_gold[i];
    cf_t value = symbols[i];

    assert(gold.real() == value.real());
    assert(gold.imag() == value.imag());
  }
}

void test_mask(unsigned nof_symbols, unsigned nof_subc, unsigned nof_elements)
{
  // Create grid and zero
  std::unique_ptr<resource_grid> grid = create_resource_grid(nof_symbols, nof_subc);
  grid->all_zero();

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
  span<const cf_t> symbol_buffer_put = symbols_gold;
  grid->put(symbol_idx, mask, symbol_buffer_put);

  // Make sure all symbols are used
  assert(symbol_buffer_put.empty());

  // Assert grid
  unsigned count = 0;
  for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
    // Get resource grid data for the given symbol
    std::vector<cf_t> rg_data(nof_subc);
    grid->get(symbol, 0, rg_data);

    for (unsigned subc = 0; subc != nof_subc; ++subc) {
      cf_t gold  = {0.0, 0.0};
      cf_t value = rg_data[subc];

      if (symbol == symbol_idx && mask[subc]) {
        gold = symbols_gold[count];
        count++;
      }

      assert(gold.real() == value.real());
      assert(gold.imag() == value.imag());
    }
  }

  // Get elements
  srsvec::aligned_vec<cf_t> symbols(nof_elements);
  span<cf_t>                symbol_buffer_get = symbols;
  grid->get(symbol_idx, mask, symbol_buffer_get);

  // Make sure all symbols are used
  assert(symbol_buffer_get.empty());

  // Assert symbols
  for (unsigned i = 0; i != nof_elements; ++i) {
    cf_t gold  = symbols_gold[i];
    cf_t value = symbols[i];

    assert(gold.real() == value.real());
    assert(gold.imag() == value.imag());
  }
}

void test_consecutive(unsigned nof_symbols, unsigned nof_subc, unsigned nof_elements)
{
  // Create grid and zero
  std::unique_ptr<resource_grid> grid = create_resource_grid(nof_symbols, nof_subc);
  grid->all_zero();

  std::uniform_int_distribution<unsigned> symbol_dist(0, nof_symbols - 1);
  std::uniform_int_distribution<unsigned> subc_dist(0, nof_subc - 1 - nof_elements);
  std::uniform_real_distribution<float>   value_dist(-1.0, +1.0);

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
  grid->put(symbol_idx, k_init, symbols_gold);

  // Assert grid
  unsigned count = 0;
  for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
    // Get resource grid data for the given symbol
    std::vector<cf_t> rg_data(nof_subc);
    grid->get(symbol, 0, rg_data);

    for (unsigned subc = 0; subc != nof_subc; ++subc) {
      cf_t gold  = {0.0, 0.0};
      cf_t value = rg_data[subc];

      if (symbol == symbol_idx && (subc >= k_init && subc < k_init + nof_elements)) {
        gold = symbols_gold[count];
        count++;
      }

      assert(gold.real() == value.real());
      assert(gold.imag() == value.imag());
    }
  }

  // Get elements
  srsvec::aligned_vec<cf_t> symbols(nof_elements);
  grid->get(symbol_idx, k_init, symbols);

  // Assert symbols
  for (unsigned i = 0; i != nof_elements; ++i) {
    cf_t gold  = symbols_gold[i];
    cf_t value = symbols[i];

    assert(gold.real() == value.real());
    assert(gold.imag() == value.imag());
  }
}

int main()
{
  for (unsigned nof_symbols : {14}) {
    for (unsigned nof_subc : {52 * 12, 270 * 12}) {
      test_all_zero(nof_symbols, nof_subc);
      for (unsigned nof_elements : {1, 2, 4, 8, 16}) {
        test_coordinates(nof_symbols, nof_subc, nof_elements);
        test_mask(nof_symbols, nof_subc, nof_elements);
        test_consecutive(nof_symbols, nof_subc, nof_elements);
      }
    }
  }

  return 0;
}