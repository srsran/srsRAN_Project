#ifndef SRSGNB_UNITTESTS_PHY_RESOURCE_GRID_TEST_DOUBLES_H_
#define SRSGNB_UNITTESTS_PHY_RESOURCE_GRID_TEST_DOUBLES_H_

#include "srsgnb/phy/resource_grid.h"
#include <vector>

namespace srsgnb {

class resource_grid_spy : public resource_grid
{
public:
  struct entry_t {
    unsigned port;  // Port index
    unsigned l;     // Symbol index
    unsigned k;     // Subcarrier index
    cf_t     value; // Complex value of the resource element
  };

  resource_grid_spy()           = default;
  ~resource_grid_spy() override = default;

  void put(unsigned port, unsigned l, unsigned k, cf_t value) { put_entries.push_back({port, l, k, value}); }
  void put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) override
  {
    const cf_t* symbol_ptr = symbols.begin();
    for (const resource_grid_coordinate& coordinate : coordinates) {
      put(port, coordinate.symbol, coordinate.subcarrier, *(symbol_ptr++));
    }
  }
  void put(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<const cf_t>& symbol_buffer) override
  {
    unsigned count = 0;
    for (unsigned k = 0; k != mask.size(); ++k) {
      if (mask[k]) {
        put(port, l, k_init + k, symbol_buffer[count]);
        count++;
      }
    }

    // consume buffer
    symbol_buffer = symbol_buffer.last(symbol_buffer.size() - count);
  }
  void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override
  {
    for (unsigned i = 0; i != symbols.size(); ++i) {
      put(port, l, k_init + i, symbols[i]);
    }
  }
  void get(unsigned port, span<const resource_grid_coordinate> coordinates, span<cf_t> symbols) const override {}
  void get(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<cf_t>& symbol_buffer) const override
  {}
  void                        get(unsigned port, unsigned l, unsigned k_init, span<cf_t> symbols) const override {}
  void                        set_all_zero() override { set_all_zero_counter++; }
  const std::vector<entry_t>& get_put_entries() const { return put_entries; }
  unsigned                    get_nof_put_entries() const { return put_entries.size(); }
  unsigned                    get_nof_set_zero_entries() const { return set_all_zero_counter; }

  void reset()
  {
    put_entries.clear();
    set_all_zero_counter = 0;
  }

private:
  std::vector<entry_t> put_entries;
  unsigned             set_all_zero_counter = 0;
};

class resource_grid_dummy : public resource_grid
{
public:
  void put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) override {}
  void put(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<const cf_t>& symbol_buffer) override
  {}
  void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override {}
  void get(unsigned port, span<const resource_grid_coordinate> coordinates, span<cf_t> symbols) const override {}
  void get(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<cf_t>& symbol_buffer) const override
  {}
  void get(unsigned port, unsigned l, unsigned k_init, span<cf_t> symbols) const override {}
  void set_all_zero() override {}
};

} // namespace srsgnb
#endif // SRSGNB_UNITTESTS_PHY_RESOURCE_GRID_TEST_DOUBLES_H_
