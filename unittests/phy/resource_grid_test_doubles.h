#ifndef SRSGNB_UNITTESTS_PHY_RESOURCE_GRID_TEST_DOUBLES_H_
#define SRSGNB_UNITTESTS_PHY_RESOURCE_GRID_TEST_DOUBLES_H_

#include "srsgnb/phy/resource_grid.h"
#include <vector>

namespace srsgnb {

class resource_grid_spy : public resource_grid_writer
{
public:
  struct entry_t {
    unsigned l;     // Symbol index
    unsigned k;     // Subcarrier index
    cf_t     value; // Complex value of the resource element
  };

  resource_grid_spy()           = default;
  ~resource_grid_spy() override = default;

  void put(unsigned l, unsigned k, cf_t value) { entries.push_back({l, k, value}); }
  void put(span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) override
  {
    const cf_t* symbol_ptr = symbols.begin();
    for (const resource_grid_coordinate& coordinate : coordinates) {
      put(coordinate.symbol, coordinate.subcarrier, *(symbol_ptr++));
    }
  }
  void put(unsigned l, span<const bool> mask, span<const cf_t>& symbol_buffer) override
  {
    unsigned count = 0;
    for (unsigned k = 0; k != mask.size(); ++k) {
      if (mask[k]) {
        put(l, k, symbol_buffer[count]);
        count++;
      }
    }

    // consume buffer
    symbol_buffer = symbol_buffer.last(symbol_buffer.size() - count);
  }
  void put(unsigned l, unsigned k_init, span<const cf_t> symbols) override
  {
    for (unsigned i = 0; i != symbols.size(); ++i) {
      put(l, k_init + i, symbols[i]);
    }
  }
  const std::vector<entry_t>& get_entries() const { return entries; }
  unsigned                    get_nof_entries() const { return entries.size(); }

private:
  std::vector<entry_t> entries;
};

class resource_grid_dummy : public resource_grid
{
public:
  void put(span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) override {}
  void put(unsigned int l, span<const bool> mask, span<const cf_t>& symbol_buffer) override {}
  void put(unsigned int l, unsigned int k_init, span<const cf_t> symbols) override {}
  void get(span<const resource_grid_coordinate> coordinates, span<cf_t> symbols) const override {}
  void get(unsigned int l, span<const bool> mask, span<cf_t>& symbol_buffer) const override {}
  void get(unsigned int l, unsigned int k_init, span<cf_t> symbols) const override {}
  void set_all_zero() override {}
};

} // namespace srsgnb
#endif // SRSGNB_UNITTESTS_PHY_RESOURCE_GRID_TEST_DOUBLES_H_
