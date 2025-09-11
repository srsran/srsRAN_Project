/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/fill.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/file_vector.h"
#include "srsran/support/srsran_assert.h"
#include "srsran/support/srsran_test.h"
#include "fmt/std.h"
#include <complex>
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
  resource_grid_writer_spy(unsigned max_ports_ = 0, unsigned max_symb_ = 0, unsigned max_prb_ = 0) :
    max_ports(max_ports_), max_symb(max_symb_), max_prb(max_prb_), data({max_prb * NRE, max_symb, max_ports})
  {
  }

  // See interface for documentation.
  unsigned get_nof_ports() const override { return max_ports; }

  // See interface for documentation.
  unsigned get_nof_subc() const override { return max_prb * NRE; }

  // See interface for documentation.
  unsigned get_nof_symbols() const override { return max_symb; }

  // See interface for documentation.
  span<const cf_t> put(unsigned                            port,
                       unsigned                            l,
                       unsigned                            k_init,
                       const bounded_bitset<NRE * MAX_RB>& mask,
                       span<const cf_t>                    symbols) override
  {
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
  span<const cbf16_t> put(unsigned                            port,
                          unsigned                            l,
                          unsigned                            k_init,
                          const bounded_bitset<NRE * MAX_RB>& mask,
                          span<const cbf16_t>                 symbols) override
  {
    ++count;
    unsigned i_symb = 0;
    for (unsigned k = 0; k != mask.size(); ++k) {
      // Skip if subcarrier is not active.
      if (!mask.test(k)) {
        continue;
      }

      // Skip Symbol if the symbol is 0.
      if (symbols[i_symb] != cbf16_t()) {
        put(port, l, k + k_init, to_cf(symbols[i_symb]));
      }

      ++i_symb;
    }

    // Consume buffer.
    return symbols.last(symbols.size() - i_symb);
  }

  // See interface for documentation.
  void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override
  {
    ++count;
    for (unsigned i = 0; i != symbols.size(); ++i) {
      put(port, l, k_init + i, symbols[i]);
    }
  }

  void put(unsigned port, unsigned l, unsigned k_init, unsigned stride, span<const cbf16_t> symbols) override
  {
    ++count;
    for (unsigned i_symb = 0; i_symb != symbols.size(); ++i_symb) {
      data[{k_init + i_symb * stride, l, port}] = symbols[i_symb];
    }
  }

  span<cbf16_t> get_view(unsigned port, unsigned l) override
  {
    ++count;
    return data.get_view({l, port});
  }

  /// \brief Asserts that the mapped resource elements match with a list of expected entries.
  ///
  /// This method asserts that mapped resource elements using the put() methods match a list of expected entries
  /// without considering any writing order.
  ///
  /// \param[in] expected_entries Provides a list of golden symbols to assert.
  /// \param[in] scaling Optional parameter that provides a scaling factor to apply on the values.
  /// \note The test is terminated in case of mismatch.
  void assert_entries(span<const expected_entry_t> expected_entries, float scaling = 1) const
  {
    // Count the number of writen RE.
    unsigned re_count = std::count_if(data.get_data().begin(), data.get_data().end(), [](cbf16_t value) {
      return (std::abs(to_cf(value)) > near_zero);
    });

    // Make sure the number of elements match.
    TESTASSERT_EQ(expected_entries.size(), re_count);

    // Iterate each expected entry, check that there is an entry and that the expected value error is below a threshold.
    for (const auto& entry : expected_entries) {
      // Extract resource element value in CBF16.
      const cbf16_t& value_cbf16 = data[{entry.subcarrier, entry.symbol, entry.port}];

      // If the entry is zero, not a number or infinite, the value in the position of the grid shall be zero.
      if (!std::isnormal(entry.value.real()) && !std::isnormal(entry.value.real())) {
        TESTASSERT(value_cbf16 == cbf16_t());
        continue;
      }

      // Verify the value is not the default.
      TESTASSERT(value_cbf16 != cbf16_t(),
                 "No resource element was written for port={}, symbol={} and subcarrier={}.",
                 entry.port,
                 entry.symbol,
                 entry.subcarrier);

      // Convert value to cf and compare with the expected value.
      cf_t  value = to_cf(value_cbf16) * scaling;
      float error = std::abs(entry.value - value);
      // Calculate maximum error allowed introduced by BFloat16 compression.
      float max_error = std::abs(entry.value) / 128.0;
      TESTASSERT(error < max_error,
                 "Mismatched value {} but expected {}. port={} symbol={} subcarrier={}.",
                 value,
                 entry.value,
                 entry.port,
                 entry.symbol,
                 entry.subcarrier);
    }
  }

  /// Get the number of times a \c put method has been called.
  unsigned get_count() const { return count; }

  bool has_grid_been_written() const { return count != 0; }

  /// Clears any possible state.
  void reset()
  {
    srsvec::zero(data.get_data());
    count = 0;
  }

private:
  /// Defines the maximum allowed error when asserting the resource grid.
  static constexpr float ASSERT_MAX_ERROR = 1e-6;
  /// Counts the number of times a \c put method is called.
  std::atomic<unsigned> count = {0};
  /// Maximum number of ports.
  unsigned max_ports;
  /// Maximum number of OFDM symbols.
  unsigned max_symb;
  /// Maximum number of RB.
  unsigned max_prb;
  /// Data storage.
  dynamic_tensor<3, cbf16_t> data;

  /// \brief This method writes a resource element in the allocation given by the port, symbol and subcarrier indexes.
  ///
  /// \param[in] port Indicates the port index.
  /// \param[in] symbol Indicates the symbol index.
  /// \param[in] subcarrier Indicates the subcarrier index.
  /// \param[in] value Indicates the RE value.
  /// \note The test is terminated in case of resource element overwrite.
  void put(uint8_t port, uint8_t symbol, uint16_t subcarrier, cf_t value)
  {
    // Ensure the port, symbol and subcarrier indexes are in range.
    TESTASSERT(port < max_ports, "Port index {} exceeded maximum {}.", port, max_ports);
    TESTASSERT(symbol < max_symb, "Symbol index {} exceeded maximum {}.", symbol, max_symb);
    TESTASSERT(subcarrier < max_prb * NRE, "Subcarrier index {} exceeded maximum {}.", subcarrier, max_prb * NRE);

    // Select reference to the resource element.
    cbf16_t& value_cbf16 = data[{subcarrier, symbol, port}];

    // Ensure the resource element does not exist.
    TESTASSERT(value_cbf16 == 0,
               "Detected resource grid overwrite for port={}, symbol={} and subcarrier={}.",
               port,
               symbol,
               subcarrier);

    // Write element.
    value_cbf16 = to_cbf16(value);
  }
};

/// Describes a resource grid reader spy.
class resource_grid_reader_spy : public resource_grid_reader
{
public:
  using expected_entry_t = resource_grid_writer_spy::expected_entry_t;

  resource_grid_reader_spy() : max_ports(0), max_symb(0), max_prb(0) {}

  resource_grid_reader_spy(unsigned max_ports_, unsigned max_symb_, unsigned max_prb_) :
    max_ports(max_ports_), max_symb(max_symb_), max_prb(max_prb_), grid({max_prb * NRE, max_symb, max_ports})
  {
    // Fill grid with NAN.
    cf_t nan = {std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()};
    srsvec::fill(grid.get_data(), to_cbf16(nan));
  }

  unsigned get_nof_ports() const override { return max_ports; }

  unsigned get_nof_subc() const override { return max_prb * NRE; }

  unsigned get_nof_symbols() const override { return max_symb; }

  bool is_empty(unsigned port) const override { return entries.empty(); }

  bool is_empty() const override { return entries.empty(); }

  span<cf_t> get(span<cf_t>                          symbols,
                 unsigned                            port,
                 unsigned                            l,
                 unsigned                            k_init,
                 const bounded_bitset<MAX_RB * NRE>& mask) const override
  {
    ++count;
    mask.for_each(0, mask.size(), [&](unsigned i_subc) {
      symbols.front() = to_cf(get(static_cast<uint8_t>(port), l, k_init + i_subc));
      symbols         = symbols.last(symbols.size() - 1);
    });

    // Consume buffer.
    return symbols;
  }

  span<cbf16_t> get(span<cbf16_t>                       symbols,
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

  void get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, unsigned stride = 1) const override
  {
    ++count;
    cf_t* symbol_ptr = symbols.data();
    for (unsigned k = k_init, k_end = k_init + stride * symbols.size(); k != k_end; k += stride) {
      *(symbol_ptr++) = to_cf(get(port, l, k));
    }
  }

  void get(span<cbf16_t> symbols, unsigned port, unsigned l, unsigned k_init) const override
  {
    ++count;
    cbf16_t* symbol_ptr = symbols.data();
    for (unsigned k = k_init, k_end = k_init + symbols.size(); k != k_end; ++k) {
      *(symbol_ptr++) = get(port, l, k);
    }
  }

  span<const cbf16_t> get_view(unsigned port, unsigned l) const override
  {
    ++count;
    return grid.get_view({l, port});
  }

  void write(span<const expected_entry_t> entries_)
  {
    unsigned current_max_sc   = max_prb * NRE;
    unsigned current_max_symb = max_symb;
    unsigned current_max_port = max_ports;
    for (const expected_entry_t& e : entries_) {
      write(e);
      if (e.subcarrier > current_max_sc) {
        current_max_sc = e.subcarrier;
      }
      if (e.symbol > current_max_symb) {
        current_max_symb = e.symbol;
      }
      if (e.port > current_max_port) {
        current_max_port = e.port;
      }
    }
    max_prb   = current_max_sc / NRE + 1;
    max_symb  = current_max_symb + 1;
    max_ports = current_max_port + 1;
  }

  void write(const expected_entry_t& entry)
  {
    entry_key_t key = {entry.port, entry.symbol, entry.subcarrier};

    entries.emplace(key, entry.value);

    if (!grid.empty()) {
      grid[{entry.subcarrier, entry.symbol, entry.port}] = entry.value;
    }
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
  std::map<entry_key_t, cbf16_t> entries;

  /// Actual grid, for the method get_view().
  dynamic_tensor<3, cbf16_t> grid;

  cbf16_t get(uint8_t port, uint8_t symbol, uint16_t subcarrier) const
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
class resource_grid_spy : public resource_grid
{
public:
  resource_grid_spy(resource_grid_reader& reader_, resource_grid_writer& writer_) : reader(reader_), writer(writer_)
  {
    // Do nothing.
  }

  void set_all_zero() override { ++set_all_zero_count; }

  void set_empty(bool empty_) { empty = empty_; }

  resource_grid_writer& get_writer() override { return writer; }

  const resource_grid_reader& get_reader() const override { return reader; }

  /// Returns true if the \c set_all_zero() method has been called, otherwise false.
  bool has_set_all_zero_method_been_called() const { return set_all_zero_count > 0; }

  /// Returns the global number of calls to any method.
  unsigned get_all_zero_count() const { return set_all_zero_count; }

  /// Resets all counters.
  void clear() { set_all_zero_count = 0; }

private:
  resource_grid_reader& reader;
  resource_grid_writer& writer;
  bool                  empty              = true;
  unsigned              set_all_zero_count = 0;
};

class shared_resource_grid_spy : private shared_resource_grid::pool_interface
{
private:
  resource_grid&        grid;
  std::atomic<unsigned> ref_count = {};

  resource_grid& get() override
  {
    srsran_assert(ref_count > 0, "The grid must be reserved.");
    return grid;
  }

  void notify_release_scope() override { srsran_assert(ref_count == 0, "The grid must be reserved."); }

public:
  explicit shared_resource_grid_spy(resource_grid& grid_) : grid(grid_) {}

  ~shared_resource_grid_spy() override
  {
    report_fatal_error_if_not(ref_count == 0, "A grid is still active in {} scopes.", ref_count);
  }

  shared_resource_grid get_grid()
  {
    unsigned expected_available_ref_count = 0;
    bool     available                    = ref_count.compare_exchange_strong(expected_available_ref_count, 1);
    srsran_assert(available, "The grid must NOT be reserved.");
    return {*this, ref_count};
  }
};

/// \brief Describes a resource grid dummy used for testing classes that handle resource grids but do not use the
/// interface.
///
/// \note The test terminates if any component under test calls any method from the interface.
class resource_grid_dummy : public resource_grid
{
private:
  resource_grid_reader_spy reader;
  resource_grid_writer_spy writer;

public:
  resource_grid_dummy() : reader(0, 0, 0), writer(0, 0, 0) {}

  unsigned set_all_zero_count = 0;

  resource_grid_writer& get_writer() override { return writer; }

  const resource_grid_reader& get_reader() const override { return reader; }

  void set_all_zero() override { ++set_all_zero_count; }

  unsigned get_reader_writer_count() const { return reader.get_count() + writer.get_count(); }

  void reset()
  {
    set_all_zero_count = 0;
    reader.reset();
    writer.reset();
  }
};

} // namespace srsran
