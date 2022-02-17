
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/srsvec/zero.h"
#include <cassert>
#include <map>
#include <random>

using namespace srsgnb;

static std::mt19937 rgen(0);

void test_put_zero(unsigned nof_symbols, unsigned nof_subc)
{
  // Create grid
  std::unique_ptr<resource_grid> grid = create_resource_grid(nof_symbols, nof_subc);

  // Set all grid to zero
  grid->all_zero();

  // Verify all RE are zero
  for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
    for (unsigned subc = 0; subc != nof_subc; ++subc) {
      cf_t re = grid->get(symbol, subc);
      assert(iszero(re.real()));
      assert(iszero(re.imag()));
    }
  }
}

void test_put_one(unsigned nof_symbols, unsigned nof_subc, unsigned nof_elements)
{
  // Create grid and zero
  std::unique_ptr<resource_grid> grid = create_resource_grid(nof_symbols, nof_subc);
  grid->all_zero();

  std::map<std::pair<unsigned, unsigned>, cf_t> data;

  std::uniform_int_distribution<unsigned> symbol_dist(0, nof_symbols - 1);
  std::uniform_int_distribution<unsigned> subc_dist(0, nof_subc - 1);
  std::uniform_real_distribution<float>   value_dist(-1.0, +1.0);

  // Put elements in grid
  for (unsigned i = 0; i != nof_elements; ++i) {
    // Create random allocation and value
    unsigned symbol = symbol_dist(rgen);
    unsigned subc   = subc_dist(rgen);
    cf_t     value  = {value_dist(rgen), value_dist(rgen)};

    // Insert in map
    data.insert({{symbol, subc}, value});

    // Put element
    grid->put(symbol, subc, value);
  }

  // Assert grid
  for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
    for (unsigned subc = 0; subc != nof_subc; ++subc) {
      cf_t gold  = {0.0, 0.0};
      cf_t value = grid->get(symbol, subc);

      std::pair<unsigned, unsigned> key = {symbol, subc};
      if (data.count(key)) {
        gold = data.at(key);
      }

      assert(gold.real() == value.real());
      assert(gold.imag() == value.imag());
    }
  }
}

void test_put_mask(unsigned nof_symbols, unsigned nof_subc, unsigned nof_elements)
{
  // Create grid and zero
  std::unique_ptr<resource_grid> grid = create_resource_grid(nof_symbols, nof_subc);
  grid->all_zero();

  std::uniform_int_distribution<unsigned> symbol_dist(0, nof_symbols - 1);
  std::uniform_int_distribution<unsigned> subc_dist(0, nof_subc - 1);
  std::uniform_real_distribution<float>   value_dist(-1.0, +1.0);

  // Put elements in grid
  unsigned                  symbol_idx = symbol_dist(rgen);
  srsvec::aligned_vec<cf_t> symbols(nof_elements);
  srsvec::aligned_vec<bool> mask(nof_subc);

  srsvec::zero(mask);

  for (unsigned i = 0; i != nof_elements; ++i) {
    // Create random allocation
    mask[subc_dist(rgen)] = true;
    symbols[i]            = {value_dist(rgen), value_dist(rgen)};
  }

  // Put element
  span<const cf_t> symbol_buffer = symbols;
  grid->put(symbol_idx, mask, symbol_buffer);

  // Make sure all elements are mapped
  assert(symbol_buffer.empty());

  // Assert grid
  unsigned count = 0;
  for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
    for (unsigned subc = 0; subc != nof_subc; ++subc) {
      cf_t gold  = {0.0, 0.0};
      cf_t value = grid->get(symbol, subc);

      if (symbol == symbol_idx && mask[subc]) {
        gold = symbols[count];
        count++;
      }

      assert(gold.real() == value.real());
      assert(gold.imag() == value.imag());
    }
  }
}

int main()
{
  for (unsigned nof_symbols : {14}) {
    for (unsigned nof_subc : {52 * 12, 270 * 12}) {
      test_put_zero(nof_symbols, nof_subc);
      for (unsigned nof_elements : {1, 2, 4, 8, 16}) {
        test_put_one(nof_symbols, nof_subc, nof_elements);
        test_put_mask(nof_symbols, nof_subc, nof_elements);
      }
    }
  }

  return 0;
}