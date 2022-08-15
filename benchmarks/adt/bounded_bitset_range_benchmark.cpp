/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "bounded_bitset_benchmark_helpers.h"
#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/support/benchmark_utils.h"
#include <getopt.h>

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
static void benchmark_bounded_bitset_anytest(const srsgnb::bounded_bitset<N>& bitset, std::string description)
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
static void benchmark_bounded_bitset_all(const srsgnb::bounded_bitset<N>& bitset, std::string description)
{
  bm->new_measure(description, params.nof_repetitions, [&bitset]() {
    bool value = bitset.all(params.start_offset, params.last_offset);
    do_not_optimize(value);
  });
}

template <unsigned N>
static void benchmark_bounded_bitset_alltest(const srsgnb::bounded_bitset<N>& bitset, std::string description)
{
  bm->new_measure(description, params.nof_repetitions, [&bitset]() {
    bool value = true;
    for (unsigned i = params.start_offset; i != params.last_offset; ++i) {
      if (not bitset.test(i)) {
        value = false;
        break;
      }
    }
    do_not_optimize(value);
  });
}

template <unsigned N>
static void benchmark_bounded_bitset_fill(srsgnb::bounded_bitset<N>& bitset, std::string description)
{
  bm->new_measure(description, params.nof_repetitions, [&bitset]() {
    bitset.fill(params.start_offset, params.last_offset);
    do_not_optimize(bitset.test(params.start_offset));
  });
}

template <unsigned N>
static void benchmark_bounded_bitset_fillset(srsgnb::bounded_bitset<N>& bitset, std::string description)
{
  bm->new_measure(description, params.nof_repetitions, [&bitset]() {
    for (unsigned i = params.start_offset; i != params.last_offset; ++i) {
      bitset.set(i);
    }
    do_not_optimize(bitset.test(params.start_offset));
  });
}

template <unsigned N>
static void benchmark_bounded_bitset_slice(srsgnb::bounded_bitset<N>& bitset, std::string description)
{
  unsigned slice_start = params.start_offset;
  unsigned slice_end   = slice_start + (params.last_offset - params.start_offset) / 2;
  bm->new_measure(description, params.nof_repetitions, [&bitset, slice_start, slice_end]() {
    bounded_bitset<N / 2> slice = bitset.template slice<N / 2>(slice_start, slice_end);
    do_not_optimize(slice.test(params.start_offset));
  });
}

template <unsigned N>
static void benchmark_bounded_bitset_slicetestset(srsgnb::bounded_bitset<N>& bitset, std::string description)
{
  unsigned slice_start = params.start_offset;
  unsigned slice_end   = slice_start + (params.last_offset - params.start_offset) / 2;
  bm->new_measure(description, params.nof_repetitions, [&bitset, slice_start, slice_end]() {
    bounded_bitset<N / 2> slice(N / 2);
    for (unsigned i = slice_start; i != slice_end; ++i) {
      if (bitset.test(i)) {
        slice.set(i - slice_start);
      }
    }
    do_not_optimize(slice.test(params.start_offset));
  });
}

template <unsigned N>
static void run_benchmark(const std::array<bool, N>& array, std::string description)
{
  bounded_bitset<N> bitset(array.begin(), array.end());

  benchmark_bounded_bitset_any<N>(bitset, "bitset:" + description + ":any");
  benchmark_bounded_bitset_anytest<N>(bitset, "bitset:" + description + ":any_test");
  benchmark_bounded_bitset_all<N>(bitset, "bitset:" + description + ":all");
  benchmark_bounded_bitset_alltest<N>(bitset, "bitset:" + description + ":all_test");
  benchmark_bounded_bitset_fill<N>(bitset, "bitset:" + description + ":fill");
  benchmark_bounded_bitset_fillset<N>(bitset, "bitset:" + description + ":fill_set");
  benchmark_bounded_bitset_slice<N>(bitset, "bitset:" + description + ":slice");
  benchmark_bounded_bitset_slicetestset<N>(bitset, "bitset:" + description + ":slice_testset");
}

int main(int argc, char** argv)
{
  constexpr size_t N = 275 * 14;

  parse_args(argc, argv, params);

  bm = std::make_unique<benchmarker>("bounded_bitset any bit set in range", params.nof_repetitions);

  run_benchmark<N>(create_contiguous_array<N>(N / 2), "contiguous");
  run_benchmark<N>(create_contiguous_array<N>(N), "all_set");
  run_benchmark<N>(create_contiguous_array<N>(0), "none");
  run_benchmark<N>(shuffle_array(create_contiguous_array<N>(N / 2)), "random");

  bm->print_percentiles_time();
}