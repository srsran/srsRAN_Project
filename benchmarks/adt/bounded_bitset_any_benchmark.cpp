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
#include <getopt.h>
#include <random>

using namespace srsgnb;

struct bench_params {
  unsigned nof_repetitions = 100000;
  unsigned start_offset    = 1;
  unsigned last_offset     = 275 * 14 - 1;
};

static void usage(const char* prog, const bench_params& params)
{
  fmt::print("Usage: {} [-R repetitions] [-b start_pos] [-e end_pos]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", params.nof_repetitions);
  fmt::print("\t-b Start offset for any check [Default {}]\n", params.start_offset);
  fmt::print("\t-e Last offset for any check [Default {}]\n", params.last_offset);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv, bench_params& params)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:b:e:h")) != -1) {
    switch (opt) {
      case 'R':
        params.nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'b':
        params.start_offset = std::strtol(optarg, nullptr, 10);
        break;
      case 'e':
        params.last_offset = std::strtol(optarg, nullptr, 10);
        break;
      case 'h':
      default:
        usage(argv[0], params);
        exit(0);
    }
  }
}

bench_params                 params{};
std::unique_ptr<benchmarker> bm;

template <unsigned N>
static void benchmark_bounded_bitset_any(const srsgnb::bounded_bitset<N>& bitset, std::string description)
{
  bm->new_measure(description, params.nof_repetitions, [&bitset]() {
    bool value = bitset.any(params.start_offset, params.last_offset);
    do_not_optimize(value);
  });
}

template <unsigned N>
static void benchmark_bounded_bitset_test(const srsgnb::bounded_bitset<N>& bitset, std::string description)
{
  bm->new_measure(description, params.nof_repetitions, [&bitset]() {
    bool value = false;
    for (unsigned i = params.start_offset; i != params.last_offset; ++i) {
      if (bitset.test(i)) {
        value = true;
        break;
      }
    }
    do_not_optimize(value);
  });
}

template <unsigned N>
static void run_benchmark(const srsgnb::bounded_bitset<N>& bitset, std::array<bool, N> array, std::string description)
{
  benchmark_bounded_bitset_any<N>(bitset, "bitset:" + description + ":any");
  benchmark_bounded_bitset_test<N>(bitset, "bitset:" + description + ":test");
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

int main(int argc, char** argv)
{
  parse_args(argc, argv, params);
  bm = std::make_unique<benchmarker>("bounded_bitset any bit set in range", params.nof_repetitions);

  benchmark_count_contiguous<275 * 14>();
  benchmark_count_all_set<275 * 14>();
  benchmark_count_none_set<275 * 14>();
  benchmark_count_random_set<275 * 14>();
  bm->print_percentiles_time();
}