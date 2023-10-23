/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

#include "srsran/adt/tensor.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/file_vector.h"
#include "srsran/support/srsran_assert.h"
#include "srsran/support/srsran_test.h"
#include <map>
#include <mutex>
#include <random>
#include <tuple>

namespace srsran {

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
  resource_grid_writer_spy(unsigned    max_ports_ = 0,
                           unsigned    max_symb_  = 0,
                           unsigned    max_prb_   = 0,
                           std::string log_level  = "none") :
    max_ports(max_ports_),
    max_symb(max_symb_),
    max_prb(max_prb_),
    logger(srslog::fetch_basic_logger("unittest/resource_grid_spy", false))
  {
    srslog::init();
    logger.set_level(srslog::str_to_basic_level(log_level));
  }

  // See interface for documentation.
  unsigned get_nof_ports() const override { return max_ports; }

  // See interface for documentation.
  unsigned get_nof_subc() const override { return max_prb * NRE; }

  // See interface for documentation.
  unsigned get_nof_symbols() const override { return max_symb; }

  // See interface for documentation.
  void put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) override
  {
    std::unique_lock<std::mutex> lock(entries_mutex);
    ++count;
    const cf_t* symbol_ptr = symbols.begin();
    for (const resource_grid_coordinate& coordinate : coordinates) {
      put(port, coordinate.symbol, coordinate.subcarrier, *(symbol_ptr++));
    }
    fmt::print("entries.size()={}\n", entries.size());
  }

  // See interface for documentation.
  span<const cf_t>
  put(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<const cf_t> symbols) override
  {
    std::unique_lock<std::mutex> lock(entries_mutex);
    TESTASSERT(k_init + mask.size() <= max_prb * NRE,
               "The mask staring at {} for {} subcarriers exceeds the resource grid bandwidth (max {}).",
               k_init,
               mask.size(),
               max_prb * NRE);
    ++count;
    unsigned i_symb = 0;
    for (unsigned k = 0; k != mask.size(); ++k) {
      if (mask[k]) {
        put(port, l, k_init + k, symbols[i_symb]);
        i_symb++;
      }
    }

    // Consume buffer.
    return symbols.last(symbols.size() - i_symb);
  }

  span<const cf_t> put(unsigned                            port,
                       unsigned                            l,
                       unsigned                            k_init,
                       const bounded_bitset<NRE * MAX_RB>& mask,
                       span<const cf_t>                    symbols) override
  {
    std::unique_lock<std::mutex> lock(entries_mutex);
    ++count;
    unsigned i_symb = 0;
    for (unsigned k = 0; k != mask.size(); ++k) {
      // Skip if subcarrier is not active.
      if (!mask.test(k)) {
        continue;
      }

      // Skip Symbol if the symbol is 0.
      if ((symbols[i_symb].real() != 0) || (symbols[i_symb].imag() != 0)) {
        put(port, l, k + k_init, symbols[i_symb]);
      }

      ++i_symb;
    }

    // Consume buffer.
    return symbols.last(symbols.size() - i_symb);
  }

  // See interface for documentation.
  void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override
  {
    std::unique_lock<std::mutex> lock(entries_mutex);
    ++count;
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
    TESTASSERT_EQ(expected_entries.size(), entries.size());

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
      TESTASSERT(err < ASSERT_MAX_ERROR, "Mismatched value {} but expected {}", value, entry.value);
    }
  }

  /// \brief Asserts that the mapped resource elements match with a list of expected entries.
  ///
  /// This method asserts that mapped resource elements using the put() methods match a list of expected entries
  /// without considering any writing order, while using a parametrizable maximkum error threshold.
  ///
  /// \param[in] expected_entries Provides a list of golden symbols to assert.
  /// \param[in] max_error Provides the maximum allowable error when comparing the data in the entries.
  /// \note The test is terminated in case of mismatch.
  void assert_entries(span<const expected_entry_t> expected_entries, float max_error) const
  {
    // Make sure the number of elements match.
    TESTASSERT_EQ(expected_entries.size(), entries.size());

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
      TESTASSERT(err < max_error, "Mismatched value {} but expected {}", value, entry.value);
    }
  }

  /// Get the number of times a \c put method has been called.
  unsigned get_count() const { return count; }

  /// Clears any possible state.
  void reset()
  {
    entries.clear();
    count = 0;
  }

private:
  /// \brief Defines the resource grid indexing key as the tuple of the port, symbol and subcarrier indexes.
  using entry_key_t = std::tuple<uint8_t, uint8_t, uint16_t>;

  /// Defines the maximum allowed error when asserting the resource grid.
  static constexpr float ASSERT_MAX_ERROR = 1e-6;

  /// Stores the resource grid written entries.
  std::map<entry_key_t, cf_t> entries;

  /// Protects concurrent write to entries.
  std::mutex entries_mutex;

  /// Counts the number of times a \c put method is called.
  unsigned count = 0;

  /// Maximum number of ports.
  unsigned max_ports;

  /// Maximum number of OFDM symbols.
  unsigned max_symb;

  /// Maximum number of RB.
  unsigned max_prb;

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

    // Ensure the port, symbol and subcarrier indexes are in range.
    TESTASSERT(port < max_ports, "Port index {} exceeded maximum {}.", port, max_ports);
    TESTASSERT(symbol < max_symb, "Symbol index {} exceeded maximum {}.", symbol, max_symb);
    TESTASSERT(subcarrier < max_prb * NRE, "Subcarrier index {} exceeded maximum {}.", subcarrier, max_prb * NRE);

    // Ensure the resource element does not exist.
    TESTASSERT(entries.count(key) == 0,
               "Detected resource grid overwrite for port={}, symbol={} and subcarrier={}.",
               port,
               symbol,
               subcarrier);

    // Debug trace.
    logger.debug("[put] port={:>2}; symbol={:>2}; subcarrier={:>4}; value={}; count={};",
                 port,
                 symbol,
                 subcarrier,
                 value,
                 entries.size() + 1);

    // Write element.
    entries.emplace(key, value);
  }
};

/// Describes a resource grid reader spy.
class resource_grid_reader_spy : public resource_grid_reader
{
public:
  using expected_entry_t = resource_grid_writer_spy::expected_entry_t;

  resource_grid_reader_spy(unsigned max_ports_ = 0, unsigned max_symb_ = 0, unsigned max_prb_ = 0) :
    max_ports(max_ports_), max_symb(max_symb_), max_prb(max_prb_)
  {
  }

  unsigned get_nof_ports() const override { return max_ports; }

  unsigned get_nof_subc() const override { return max_prb * NRE; }

  unsigned get_nof_symbols() const override { return max_symb; }

  bool is_empty(unsigned port) const override { return entries.empty(); }

  span<cf_t> get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, span<const bool> mask) const override
  {
    ++count;
    unsigned i_symb = 0;
    for (unsigned k = 0; k != mask.size(); ++k) {
      if (mask[k]) {
        symbols[i_symb] = get(static_cast<uint8_t>(port), l, k_init + k);
        i_symb++;
      }
    }

    // Consume buffer.
    return symbols.last(symbols.size() - i_symb);
  }

  span<cf_t> get(span<cf_t>                          symbols,
                 unsigned                            port,
                 unsigned                            l,
                 unsigned                            k_init,
                 const bounded_bitset<MAX_RB * NRE>& mask) const override
  {
    ++count;
    mask.for_each(0, mask.size(), [&](unsigned i_subc) {
      symbols.front() = get(static_cast<uint8_t>(port), l, k_init + i_subc);
      symbols         = symbols.last(symbols.size() - 1);
    });

    // Consume buffer.
    return symbols;
  }

  void get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init) const override
  {
    ++count;
    cf_t* symbol_ptr = symbols.data();
    for (unsigned k = k_init, k_end = k_init + symbols.size(); k != k_end; ++k) {
      *(symbol_ptr++) = get(port, l, k);
    }
  }

  span<const cf_t> get_view(unsigned port, unsigned l) const override
  {
    srsran_assert(false, "Unimplemented method");
    return {};
  }

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

  unsigned get_count() const { return count; }

  /// Clears any possible state.
  void reset()
  {
    entries.clear();
    count = 0;
  }

private:
  unsigned         max_ports;
  unsigned         max_symb;
  unsigned         max_prb;
  mutable unsigned count = 0;

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
                  "Resource grid for port={}, symbol={} and subcarrier={} does not exist.",
                  port,
                  symbol,
                  subcarrier);

    return entries.at(key);
  }
};

/// Describes a resource grid spy.
class resource_grid_spy : public resource_grid, private resource_grid_mapper
{
public:
  resource_grid_spy(unsigned max_ports = 0, unsigned max_symb = 0, unsigned max_prb = 0) :
    reader(max_ports, max_symb, max_prb), writer(max_ports, max_symb, max_prb)
  {
    // Do nothing.
  }

  void set_all_zero() override
  {
    ++set_all_zero_count;

    // Reset the reader and writer.
    reader.reset();
    writer.reset();
  }

  void set_empty(bool empty_) { empty = empty_; }

  resource_grid_writer& get_writer() override { return writer; }

  const resource_grid_reader& get_reader() const override { return reader; }

  /// Returns true if the \c set_all_zero() method has been called, otherwise false.
  bool has_set_all_zero_method_been_called() const { return set_all_zero_count > 0; }

  /// Returns the global number of calls to any method.
  unsigned get_total_count() const { return set_all_zero_count + reader.get_count() + writer.get_count(); }

  /// Resets all counters.
  void clear()
  {
    set_all_zero_count = 0;
    reader.reset();
    writer.reset();
  }

  resource_grid_mapper& get_mapper() override { return *this; }

  void map(const re_buffer_reader& /* input */,
           const re_pattern_list& /* pattern */,
           const re_pattern_list& /* reserved */,
           const precoding_configuration& /* precoding */) override
  {
    srsran_assertion_failure("Resource grid spy does not implement the resource grid mapper.");
  }

  void map(const re_buffer_reader& /* input */,
           const re_pattern_list& /* pattern */,
           const precoding_configuration& /* precoding */) override
  {
    srsran_assertion_failure("Resource grid spy does not implement the resource grid mapper.");
  }

  void map(symbol_buffer&                 buffer,
           const re_pattern_list&         pattern,
           const re_pattern_list&         reserved,
           const precoding_configuration& precoding) override
  {
    srsran_assertion_failure("Resource grid spy does not implement the resource grid mapper.");
  }

private:
  resource_grid_reader_spy reader;
  resource_grid_writer_spy writer;
  bool                     empty              = true;
  unsigned                 set_all_zero_count = 0;
};

/// \brief Describes a resource grid dummy used for testing classes that handle resource grids but do not use the
/// interface.
///
/// \note The test terminates if any component under test calls any method from the interface.
class resource_grid_dummy : public resource_grid, private resource_grid_mapper
{
private:
  /// Throws a assertion failure due to an overridden method call.
  void failure() const
  {
    srsran_assertion_failure(
        "Components using resource grid dummy are not allowed to call any method from the interface.");
  }

  resource_grid_reader_spy reader;
  resource_grid_writer_spy writer;

public:
  resource_grid_dummy() : reader(0, 0, 0), writer(0, 0, 0) {}

  unsigned set_all_zero_count = 0;

  resource_grid_writer& get_writer() override { return writer; }

  const resource_grid_reader& get_reader() const override { return reader; }

  void set_all_zero() override { ++set_all_zero_count; }

  resource_grid_mapper& get_mapper() override { return *this; }

  void map(const re_buffer_reader& /* input */,
           const re_pattern_list& /* pattern */,
           const re_pattern_list& /* reserved */,
           const precoding_configuration& /* precoding */) override
  {
    failure();
  }

  void
  map(const re_buffer_reader& input, const re_pattern_list& pattern, const precoding_configuration& precoding) override
  {
    failure();
  }

  void map(symbol_buffer& /* buffer */,
           const re_pattern_list& /* pattern */,
           const re_pattern_list& /* reserved */,
           const precoding_configuration& /* precoding */) override
  {
    failure();
  }

  unsigned get_reader_writer_count() const { return reader.get_count() + writer.get_count(); }

  void reset()
  {
    set_all_zero_count = 0;
    reader.reset();
    writer.reset();
  }
};

} // namespace srsran
