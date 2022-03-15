#ifndef SRSGNB_UNITTESTS_PHY_RESOURCE_GRID_TEST_DOUBLES_H_
#define SRSGNB_UNITTESTS_PHY_RESOURCE_GRID_TEST_DOUBLES_H_

#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/support/srsran_assert.h"
#include <map>

namespace srsgnb {

/// Describes a resource grid writer spy.
class resource_grid_writer_spy : public resource_grid_writer
{
public:
  /// Describes an expected resource grid writer spy entry.
  struct expected_entry_t {
    /// Indicates the port index.
    unsigned port;
    /// Indicates the symbol index.
    unsigned symbol;
    /// Indicates the subcarrier index.
    unsigned subcarrier;
    /// Provides the complex resource element value
    cf_t value;
  };

  /// Describes the resource grid key to index values in the grid map.
  struct entry_key_t {
    /// Indicates the port index.
    unsigned port;
    /// Indicates the symbol index.
    unsigned symbol;
    /// Indicates the subcarrier index.
    unsigned subcarrier;

    /// Implement lower than comparison to enable \c std::map indexing. The mapping order is port, symbol and subcarrier
    /// indexes.
    bool operator<(const entry_key_t& other) const
    {
      if (port != other.port) {
        return port < other.port;
      }

      if (symbol != other.symbol) {
        return symbol < other.symbol;
      }

      return subcarrier < other.subcarrier;
    }
  };

  void put(unsigned port, unsigned l, unsigned k, cf_t value) { entries.emplace(entry_key_t{port, l, k}, value); }
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

    // Consume buffer.
    symbol_buffer = symbol_buffer.last(symbol_buffer.size() - count);
  }
  void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override
  {
    for (unsigned i = 0; i != symbols.size(); ++i) {
      put(port, l, k_init + i, symbols[i]);
    }
  }

  /// \brief Asserts that the mapped resource elements match with a list of expected entries.
  ///
  /// This method asserts that mapped resource elements using the \c put methods match a list of expected entries
  /// without considering any writing order.
  ///
  /// \param[in] golden_entries Provides a list of golden symbols to assert.
  /// \note The test is terminated in case of miss-match.
  void assert_entries(span<const expected_entry_t> expected_entries) const
  {
    // Make sure the number of elements match.
    srsran_assert(entries.size() == expected_entries.size(),
                  "The number of expected entries (%d) is not equal to the number of entries (%d).",
                  expected_entries.size(),
                  entries.size());

    // Iterate each expected entry check that there is an entry and the expected value error is below a threshold.
    for (const auto& entry : expected_entries) {
      entry_key_t key = {entry.port, entry.symbol, entry.subcarrier};
      srsran_assert(entries.count(key), "No resource element was written for port=%d, symbol=%d and subcarrier=%d.");

      cf_t  value = entries.at(key);
      float err   = std::abs(entry.value - value);
      srsran_assert(err < assert_max_error,
                    "Mismatched value %+f%+f but expected %+f%+f",
                    value.real(),
                    value.imag(),
                    entry.value.real(),
                    entry.value.imag());
    }
  }

  /// Clears any possible state.
  void reset() { entries.clear(); }

private:
  /// Defines the maximum allowed error when asserting the resource grid.
  static constexpr float assert_max_error = 1e-6;

  /// Stores the resource grid written entries.
  std::map<entry_key_t, cf_t> entries;
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
