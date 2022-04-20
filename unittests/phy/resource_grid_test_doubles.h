#ifndef SRSGNB_UNITTESTS_PHY_RESOURCE_GRID_TEST_DOUBLES_H_
#define SRSGNB_UNITTESTS_PHY_RESOURCE_GRID_TEST_DOUBLES_H_

#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/support/srsgnb_test.h"
#include "srsgnb/support/srsran_assert.h"
#include <map>
#include <random>
#include <tuple>

namespace srsgnb {

/// Describes a resource grid writer spy for testing classes that write in the resource grid.
class resource_grid_writer_spy : public resource_grid_writer
{
public:
  /// Describes an expected resource grid writer spy entry.
  struct expected_entry_t {
    /// Indicates the port index.
    uint8_t port;
    /// Indicates the symbol index.
    uint8_t symbol;
    /// Indicates the subcarrier index.
    uint16_t subcarrier;
    /// Provides the complex resource element value.
    cf_t value;
  };

  /// Constructs a resource spy.
  resource_grid_writer_spy(std::string log_level = "none") :
    logger(srslog::fetch_basic_logger("unittest/resource_grid_spy", false))
  {
    srslog::init();
    logger.set_level(srslog::str_to_basic_level(log_level));
  }

  // See interface for documentation.
  void put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) override
  {
    const cf_t* symbol_ptr = symbols.begin();
    for (const resource_grid_coordinate& coordinate : coordinates) {
      put(port, coordinate.symbol, coordinate.subcarrier, *(symbol_ptr++));
    }
  }

  // See interface for documentation.
  span<const cf_t>
  put(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<const cf_t> symbols) override
  {
    unsigned count = 0;
    for (unsigned k = 0; k != mask.size(); ++k) {
      if (mask[k]) {
        put(port, l, k_init + k, symbols[count]);
        count++;
      }
    }

    // Consume buffer.
    return symbols.last(symbols.size() - count);
  }

  // See interface for documentation.
  void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override
  {
    for (unsigned i = 0; i != symbols.size(); ++i) {
      put(port, l, k_init + i, symbols[i]);
    }
  }

  /// \brief Asserts that the mapped resource elements match with a list of expected entries.
  ///
  /// This method asserts that mapped resource elements using the put() methods match a list of expected entries
  /// without considering any writing order.
  ///
  /// \param[in] expected_entries Provides a list of golden symbols to assert.
  /// \note The test is terminated in case of mismatch.
  void assert_entries(span<const expected_entry_t> expected_entries) const
  {
    // Make sure the number of elements match.
    TESTASSERT_EQ(entries.size(), expected_entries.size());

    // Iterate each expected entry, check that there is an entry and that the expected value error is below a threshold.
    for (const auto& entry : expected_entries) {
      entry_key_t key = {entry.port, entry.symbol, entry.subcarrier};
      TESTASSERT(entries.count(key),
                 "No resource element was written for port={}, symbol={} and subcarrier={}.",
                 entry.port,
                 entry.symbol,
                 entry.subcarrier);

      cf_t  value = entries.at(key);
      float err   = std::abs(entry.value - value);
      TESTASSERT(err < ASSERT_MAX_ERROR,
                 "Mismatched value {:+f}{:+f} but expected {:+f}{:+f}",
                 value.real(),
                 value.imag(),
                 entry.value.real(),
                 entry.value.imag());
    }
  }

  /// Clears any possible state.
  void reset() { entries.clear(); }

private:
  /// \brief Defines the resource grid indexing key as the tuple of the port, symbol and subcarrier indexes.
  using entry_key_t = std::tuple<uint8_t, uint8_t, uint16_t>;

  /// Defines the maximum allowed error when asserting the resource grid.
  static constexpr float ASSERT_MAX_ERROR = 1e-6;

  /// Stores the resource grid written entries.
  std::map<entry_key_t, cf_t> entries;

  /// Stores logger.
  srslog::basic_logger& logger;

  /// \brief This method writes a resource element in the allocation given by the port, symbol and subcarrier indexes.
  ///
  /// \param[in] port Indicates the port index.
  /// \param[in] symbol Indicates the symbol index.
  /// \param[in] subcarrier Indicates the subcarrier index.
  /// \param[in] value Indicates the RE value.
  /// \note The test is terminated in case of resource element overwrite.
  void put(uint8_t port, uint8_t symbol, uint16_t subcarrier, cf_t value)
  {
    // Generate key.
    entry_key_t key{port, symbol, subcarrier};

    // Ensure the resource element does not exist.
    srsran_assert(entries.count(key) == 0,
                  "Detected resource grid overwrite for port=%d, symbol=%d and subcarrier=%d.",
                  port,
                  symbol,
                  subcarrier);

    // Debug trace.
    logger.debug("[put] port={:>2}; symbol={:>2}; subcarrier={:>4}; value={:+}{:+}j; count={};",
                 port,
                 symbol,
                 subcarrier,
                 value.real(),
                 value.imag(),
                 entries.size() + 1);

    // Write element.
    entries.emplace(key, value);
  }
};

/// Describes a resource grid reader spy.
class resource_grid_reader_spy : public resource_grid_reader
{
public:
  /// Describes an expected resource grid reader spy entry.
  struct expected_entry_t {
    /// Indicates the port index.
    uint8_t port;
    /// Indicates the symbol index.
    uint8_t symbol;
    /// Indicates the subcarrier index.
    uint16_t subcarrier;
    /// Provides the complex resource element value.
    cf_t value;
  };

  void get(span<cf_t> symbols, unsigned port, span<const resource_grid_coordinate> coordinates) const override
  {
    srsran_always_assert(symbols.size() == coordinates.size(), "Number of symbols and coordinates must be the equal.");
    for (unsigned idx = 0; idx != coordinates.size(); ++idx) {
      symbols[idx] = get(static_cast<uint8_t>(port), coordinates[idx].symbol, coordinates[idx].subcarrier);
    }
  }
  span<cf_t> get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, span<const bool> mask) const override
  {
    return span<cf_t>();
  }
  void get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init) const override {}

  void write(span<const expected_entry_t> entries_)
  {
    for (const expected_entry_t& e : entries_) {
      write(e);
    }
  }

  void write(const expected_entry_t& entry)
  {
    entry_key_t key = {entry.port, entry.symbol, entry.subcarrier};

    entries.emplace(key, entry.value);
  }

private:
  /// \brief Defines the resource grid indexing key as the tuple of the port, symbol and subcarrier indexes.
  using entry_key_t = std::tuple<uint8_t, uint8_t, uint16_t>;

  /// Stores the resource grid written entries.
  std::map<entry_key_t, cf_t> entries;

  cf_t get(uint8_t port, uint8_t symbol, uint16_t subcarrier) const
  {
    // Generate key.
    entry_key_t key{port, symbol, subcarrier};

    // Ensure the resource element exist.
    srsran_assert(entries.count(key) == 1,
                  "Resource grid for port=%d, symbol=%d and subcarrier=%d does not exist.",
                  port,
                  symbol,
                  subcarrier);

    return entries.at(key);
  }
};

/// \brief Describes a resource grid dummy used for testing classes that handle resource grids but do not use the
/// interface.
///
/// \note The test terminates if any component under test calls any method from the interface.
class resource_grid_dummy : public resource_grid
{
private:
  /// Throws a assertion failure due to an overridden method call.
  void failure() const
  {
    srsran_assertion_failure(
        "Components using resource grid dummy are not allowed to call any method from the interface.");
  }

public:
  void put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) override
  {
    failure();
  }
  span<const cf_t>
  put(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<const cf_t> symbols) override
  {
    failure();
    return {};
  }
  void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override { failure(); }
  void get(span<cf_t> symbols, unsigned port, span<const resource_grid_coordinate> coordinates) const override
  {
    failure();
  }
  span<cf_t> get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, span<const bool> mask) const override
  {
    failure();
    return {};
  }
  void get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init) const override { failure(); }
  void set_all_zero() override { failure(); }
};

} // namespace srsgnb
#endif // SRSGNB_UNITTESTS_PHY_RESOURCE_GRID_TEST_DOUBLES_H_
