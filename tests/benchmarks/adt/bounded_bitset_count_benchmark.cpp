/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "bounded_bitset_benchmark_helpers.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/support/benchmark_utils.h"
#include <getopt.h>

using namespace srsran;

unsigned nof_repetitions = 100000;

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-R repetitions] [-s silent]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:h")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'h':
      default:
        usage(argv[0]);
        exit(0);
    }
  }
}

std::unique_ptr<benchmarker> bm;

template <unsigned N>
static void benchmark_bounded_bitset_count(const srsran::bounded_bitset<N>& bitset, std::string description)
{
  bm->new_measure(description, nof_repetitions, [&bitset]() {
    unsigned count = bitset.count();
    do_not_optimize(count);
  });
}

template <unsigned N>
static void benchmark_bounded_bitset_test(const srsran::bounded_bitset<N>& bitset, std::string description)
{
  bm->new_measure(description, nof_repetitions, [&bitset]() {
    unsigned count = 0;
    for (unsigned i = 0; i != N; ++i) {
      if (bitset.test(i)) {
        count++;
      }
    }
    do_not_optimize(count);
  });
}

template <size_t N>
class bounded_bitset_iterator
{
private:
  const bounded_bitset<N>& bitset;
  bool                     value;
  size_t                   current_pos;
  size_t                   burst_end;

  void next_burst()
  {
    int pos = bitset.find_lowest(burst_end, bitset.size(), value);
    if (pos == -1) {
      current_pos = bitset.size();
      burst_end   = current_pos;
      return;
    }
    current_pos = static_cast<size_t>(pos);

    pos = bitset.find_lowest(current_pos, bitset.size(), not value);
    if (pos == -1) {
      burst_end = bitset.size();
    } else {
      burst_end = static_cast<size_t>(pos);
    }
  }

public:
  bounded_bitset_iterator(const bounded_bitset<N>& bitset_) :
    bitset(bitset_), value(true), current_pos(bitset.size()), burst_end(bitset.size())
  {
  }

  bounded_bitset_iterator(const bounded_bitset<N>& bitset_, bool value_) :
    bitset(bitset_), value(value_), current_pos(0), burst_end(0)
  {
    next_burst();
  }

  size_t operator*() const noexcept { return current_pos; }

  bounded_bitset_iterator& operator++()
  {
    if (current_pos >= bitset.size()) {
      return *this;
    }

    ++current_pos;
    if (current_pos == burst_end) {
      next_burst();
    }

    return *this;
  }

  bool operator==(const bounded_bitset_iterator& other) { return (current_pos == other.current_pos); }

  bool operator!=(const bounded_bitset_iterator& other) { return !(*this == other); }
};

template <size_t N>
class bounded_bitset_iterable
{
private:
  const bounded_bitset<N>& bitset;
  bool                     value;

public:
  bounded_bitset_iterable(const bounded_bitset<N>& bitset_, bool value_ = true) : bitset(bitset_), value(value_) {}

  constexpr bounded_bitset_iterator<N> begin() const { return bounded_bitset_iterator<N>(bitset, value); }

  constexpr bounded_bitset_iterator<N> end() const { return bounded_bitset_iterator<N>(bitset); }
};

template <unsigned N>
static void benchmark_bounded_bitset_foreach(const srsran::bounded_bitset<N>& bitset, std::string description)
{
  bm->new_measure(description, nof_repetitions, [&bitset]() {
    unsigned count = 0;
    bitset.for_each(0, bitset.size(), [&count](size_t i) { count++; });
    do_not_optimize(count);
  });
}

template <unsigned N>
static void benchmark_bounded_bitset_iterator(const srsran::bounded_bitset<N>& bitset, std::string description)
{
  bm->new_measure(description, nof_repetitions, [&bitset]() {
    unsigned                   count = 0;
    bounded_bitset_iterable<N> iterable(bitset);
    for (bounded_bitset_iterator<N> it = iterable.begin(), end = iterable.end(); it != end; ++it) {
      count++;
    }
    do_not_optimize(count);
  });
}

template <unsigned N>
static void benchmark_array_count(const std::array<bool, N>& array, std::string description)
{
  bm->new_measure(description, nof_repetitions, [&array]() {
    unsigned count = std::count(array.begin(), array.end(), true);
    do_not_optimize(count);
  });
}

template <unsigned N>
static void run_benchmark(std::array<bool, N> array, std::string description)
{
  srsran::bounded_bitset<N> bitset(array.begin(), array.end());

  benchmark_bounded_bitset_count<N>(bitset, "bitset:" + description + ":count");
  benchmark_bounded_bitset_test<N>(bitset, "bitset:" + description + ":test");
  benchmark_bounded_bitset_iterator<N>(bitset, "bitset:" + description + ":iterator");
  benchmark_bounded_bitset_foreach<N>(bitset, "bitset:" + description + ":foreach");
  benchmark_array_count<N>(array, "array:" + description + ":count");
}

int main(int argc, char** argv)
{
  constexpr size_t N = 275 * 14;

  parse_args(argc, argv);

  bm = std::make_unique<benchmarker>("bounded_bitset count ones", nof_repetitions);

  run_benchmark<N>(create_contiguous_array<N>(N / 2), "contiguous");
  run_benchmark<N>(create_contiguous_array<N>(N), "all_set");
  run_benchmark<N>(create_contiguous_array<N>(0), "none");
  run_benchmark<N>(shuffle_array(create_contiguous_array<N>(N / 2)), "random");
  bm->print_percentiles_time();
}
