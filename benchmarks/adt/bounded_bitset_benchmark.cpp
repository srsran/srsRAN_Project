/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/support/benchmark_utils.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

using namespace srsgnb;

static constexpr unsigned iterations = 10000;

benchmarker bm("bounded_bitset count ones", iterations);

template <unsigned N>
static void benchmark_bounded_bitset_count(const srsgnb::bounded_bitset<N>& bitset, std::string description)
{
  unsigned expected  = bitset.count();
  unsigned count_sum = 0;

  bm.new_measure(description, iterations, [&count_sum, &bitset]() { count_sum += bitset.count(); });

  TESTASSERT_EQ(expected, count_sum / iterations);
}

template <unsigned N>
static void benchmark_bounded_bitset_test(const srsgnb::bounded_bitset<N>& bitset, std::string description)
{
  unsigned expected  = bitset.count();
  unsigned count_sum = 0;

  bm.new_measure(description, iterations, [&count_sum, &bitset]() {
    unsigned count = 0;
    for (unsigned i = 0; i != N; ++i) {
      if (bitset.test(i)) {
        count++;
      }
    }
    count_sum += count;
  });

  TESTASSERT_EQ(expected, count_sum / iterations);
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
static void benchmark_bounded_bitset_foreach(const srsgnb::bounded_bitset<N>& bitset, std::string description)
{
  unsigned expected  = bitset.count();
  unsigned count_sum = 0;

  bm.new_measure(description, iterations, [&count_sum, &bitset]() {
    unsigned count = 0;
    bitset.for_each(0, bitset.size(), [&count](size_t i) { count++; });
    count_sum += count;
  });

  TESTASSERT_EQ(expected, count_sum / iterations);
}

template <unsigned N>
static void benchmark_bounded_bitset_iterator(const srsgnb::bounded_bitset<N>& bitset, std::string description)
{
  unsigned expected  = bitset.count();
  unsigned count_sum = 0;

  bm.new_measure(description, iterations, [&count_sum, &bitset]() {
    unsigned                   count = 0;
    bounded_bitset_iterable<N> iterable(bitset);
    for (bounded_bitset_iterator<N> it = iterable.begin(), end = iterable.end(); it != end; ++it) {
      count++;
    }
    count_sum += count;
  });

  TESTASSERT_EQ(expected, count_sum / iterations);
}

template <unsigned N>
static void benchmark_array_count(const std::array<bool, N>& array, std::string description)
{
  unsigned expected  = std::count(array.begin(), array.end(), true);
  unsigned count_sum = 0;

  bm.new_measure(description, iterations, [&count_sum, &array]() {
    unsigned count = std::count(array.begin(), array.end(), true);
    count_sum += count;
  });
  TESTASSERT_EQ(expected, count_sum / iterations);
}

template <unsigned N>
static void run_benchmark(const srsgnb::bounded_bitset<N>& bitset, std::array<bool, N> array, std::string description)
{
  benchmark_bounded_bitset_count<N>(bitset, "bitset:" + description + ":count");
  benchmark_bounded_bitset_test<N>(bitset, "bitset:" + description + ":test");
  benchmark_bounded_bitset_iterator<N>(bitset, "bitset:" + description + ":iterator");
  benchmark_bounded_bitset_foreach<N>(bitset, "bitset:" + description + ":foreach");
  benchmark_array_count<N>(array, "array:" + description + ":count");
}

template <unsigned N>
static void benchmark_count_contiguous()
{
  srsgnb::bounded_bitset<N> bitset(N);
  std::array<bool, N>       array = {};
  for (unsigned i = 0; i != N; ++i) {
    if (i < N / 2) {
      bitset.set(i);
      array[i] = true;
    }
  }

  run_benchmark<N>(bitset, array, "contiguous");
}

template <unsigned N>
static void benchmark_count_all_set()
{
  srsgnb::bounded_bitset<N> bitset(N);
  std::array<bool, N>       array = {};
  for (unsigned i = 0; i != N; ++i) {
    bitset.set(i);
    array[i] = true;
  }

  run_benchmark<N>(bitset, array, "all_set");
}

template <unsigned N>
static void benchmark_count_none_set()
{
  srsgnb::bounded_bitset<N> bitset(N);
  std::array<bool, N>       array = {};

  run_benchmark<N>(bitset, array, "none");
}

template <unsigned N>
static void benchmark_count_random_set()
{
  std::mt19937                            rgen;
  std::uniform_int_distribution<unsigned> dist(0, N - 1);
  srsgnb::bounded_bitset<N>               bitset(N);
  std::array<bool, N>                     array = {};

  for (unsigned i = 0; i != N / 2; ++i) {
    unsigned pos;
    do {
      pos = dist(rgen);
    } while (array[pos]);

    bitset.set(pos);
    array[pos] = true;
  }

  run_benchmark<N>(bitset, array, "random");
}

int main()
{
  benchmark_count_contiguous<275 * 12>();
  benchmark_count_all_set<275 * 12>();
  benchmark_count_none_set<275 * 12>();
  benchmark_count_random_set<275 * 12>();
  bm.print_percentiles_time();
}
